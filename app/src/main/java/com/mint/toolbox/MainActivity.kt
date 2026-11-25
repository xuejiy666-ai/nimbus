package com.mint.toolbox

import android.app.Activity
import android.content.ClipData
import android.content.ClipboardManager
import android.content.Intent
import android.net.Uri
import android.os.Bundle
import android.provider.OpenableColumns
import android.view.View
import android.widget.Button
import android.widget.LinearLayout
import android.widget.ScrollView
import android.widget.TextView
import android.widget.Toast
import java.io.File
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale

class MainActivity : Activity() {

    private lateinit var btnPick: Button
    private lateinit var btnStart: Button
    private lateinit var btnCopy: Button
    private lateinit var tvFileName: TextView
    private lateinit var tvFileMeta: TextView
    private lateinit var tvLog: TextView
    private lateinit var tvProgress: TextView
    private lateinit var progressRow: LinearLayout
    private lateinit var cardResult: LinearLayout
    private lateinit var tvResultPath: TextView
    private lateinit var logScroll: ScrollView

    private var pickedUri: Uri? = null
    private var pickedName: String = ""
    private var running: Boolean = false

    private val clock = SimpleDateFormat("HH:mm:ss", Locale.getDefault())

    override fun onCreate(state: Bundle?) {
        super.onCreate(state)
        setContentView(R.layout.activity_main)

        btnPick = findViewById(R.id.btnPick)
        btnStart = findViewById(R.id.btnStart)
        btnCopy = findViewById(R.id.btnCopy)
        tvFileName = findViewById(R.id.tvFileName)
        tvFileMeta = findViewById(R.id.tvFileMeta)
        tvLog = findViewById(R.id.tvLog)
        tvProgress = findViewById(R.id.tvProgress)
        progressRow = findViewById(R.id.progressRow)
        cardResult = findViewById(R.id.cardResult)
        tvResultPath = findViewById(R.id.tvResultPath)
        logScroll = findViewById(R.id.logScroll)

        btnPick.setOnClickListener { pickApk() }
        btnStart.setOnClickListener { startProcessing() }
        btnCopy.setOnClickListener { copyResultPath() }

        setStartEnabled(false)
        log("就绪，先选一个 APK，再点「开始处理」。")
    }

    private fun pickApk() {
        if (running) return
        val i = Intent(Intent.ACTION_OPEN_DOCUMENT)
        i.addCategory(Intent.CATEGORY_OPENABLE)
        i.type = "*/*"
        i.putExtra(
            Intent.EXTRA_MIME_TYPES,
            arrayOf("application/vnd.android.package-archive", "application/octet-stream")
        )
        startActivityForResult(i, REQ_PICK)
    }

    override fun onActivityResult(req: Int, res: Int, data: Intent?) {
        super.onActivityResult(req, res, data)
        if (req != REQ_PICK || res != RESULT_OK) return
        val uri = data?.data ?: return

        var name = uri.lastPathSegment ?: "input.apk"
        var size = -1L
        try {
            contentResolver.query(uri, null, null, null, null)?.use { c ->
                val nameIdx = c.getColumnIndex(OpenableColumns.DISPLAY_NAME)
                val sizeIdx = c.getColumnIndex(OpenableColumns.SIZE)
                if (c.moveToFirst()) {
                    if (nameIdx >= 0 && !c.isNull(nameIdx)) name = c.getString(nameIdx) ?: name
                    if (sizeIdx >= 0 && !c.isNull(sizeIdx)) size = c.getLong(sizeIdx)
                }
            }
        } catch (_: Throwable) {
        }

        pickedUri = uri
        pickedName = name
        tvFileName.text = name
        tvFileMeta.text = if (size > 0) human(size) + " · 待处理" else "待处理"
        setStartEnabled(true)
        log("已选择：$name" + if (size > 0) "（${human(size)}）" else "")
    }

    private fun startProcessing() {
        val uri = pickedUri ?: return
        if (running) return
        val name = pickedName
        running = true
        btnPick.isEnabled = false
        btnPick.alpha = 0.45f
        btnStart.isEnabled = false
        btnStart.alpha = 0.45f
        progressRow.visibility = View.VISIBLE
        tvProgress.text = "正在处理…"
        cardResult.visibility = View.GONE
        log("开始处理：$name")

        Thread {
            val stage = File(cacheDir, "in.apk")
            var ok = false
            try {
                contentResolver.openInputStream(uri)?.use { input ->
                    stage.outputStream().use { out -> input.copyTo(out) }
                } ?: throw IllegalStateException("无法读取该文件")
                uiLog("已复制到私有目录（${human(stage.length())}）")

                val outDir = (getExternalFilesDir("Dex") ?: filesDir).absolutePath
                val started = System.currentTimeMillis()
                val result = Native.run(stage.absolutePath, outDir)
                val cost = (System.currentTimeMillis() - started) / 1000.0

                uiLog(String.format(Locale.US, "原生层返回 %d 条结果，耗时 %.1fs", result.size, cost))
                for (line in result) uiLog("· $line")
                if (result.isEmpty()) uiLog("没有取到 dex，可能不是受支持的加固方案")
                uiLog("输出目录：$outDir")
                ok = true
                runOnUiThread {
                    tvResultPath.text = outDir
                    cardResult.visibility = View.VISIBLE
                }
            } catch (t: Throwable) {
                uiLog("失败：" + (t.message ?: t.javaClass.simpleName))
            } finally {
                stage.delete()
                running = false
                runOnUiThread {
                    btnPick.isEnabled = true
                    btnPick.alpha = 1f
                    btnStart.isEnabled = pickedUri != null
                    btnStart.alpha = if (btnStart.isEnabled) 1f else 0.45f
                    progressRow.visibility = View.GONE
                    Toast.makeText(
                        this,
                        if (ok) "处理完成" else "处理失败，见运行日志",
                        Toast.LENGTH_SHORT
                    ).show()
                    if (ok) log("完成")
                }
            }
        }.start()
    }

    private fun copyResultPath() {
        val p = tvResultPath.text?.toString().orEmpty()
        if (p.isEmpty()) return
        val cm = getSystemService(CLIPBOARD_SERVICE) as ClipboardManager
        cm.setPrimaryClip(ClipData.newPlainText("dex path", p))
        Toast.makeText(this, "路径已复制", Toast.LENGTH_SHORT).show()
    }

    private fun uiLog(msg: String) {
        runOnUiThread { log(msg) }
    }

    private fun log(msg: String) {
        tvLog.append(clock.format(Date()) + "  " + msg + "\n")
        logScroll.post { logScroll.fullScroll(View.FOCUS_DOWN) }
    }

    private fun setStartEnabled(on: Boolean) {
        btnStart.isEnabled = on
        btnStart.alpha = if (on) 1f else 0.45f
    }

    private fun human(bytes: Long): String {
        if (bytes < 1024) return "$bytes B"
        val kb = bytes / 1024.0
        if (kb < 1024) return String.format(Locale.US, "%.1f KB", kb)
        return String.format(Locale.US, "%.2f MB", kb / 1024)
    }

    companion object {
        private const val REQ_PICK = 1
    }
}
