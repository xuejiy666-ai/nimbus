package com.mint.toolbox

import android.app.Activity
import android.content.Intent
import android.os.Bundle
import android.widget.Button
import java.io.File

class MainActivity : Activity() {

    override fun onCreate(state: Bundle?) {
        super.onCreate(state)
        val b = Button(this)
        b.text = "APK"
        b.setOnClickListener {
            val i = Intent(Intent.ACTION_OPEN_DOCUMENT)
            i.type = "*/*"
            i.addCategory(Intent.CATEGORY_OPENABLE)
            startActivityForResult(i, 1)
        }
        setContentView(b)
    }

    override fun onActivityResult(req: Int, res: Int, data: Intent?) {
        super.onActivityResult(req, res, data)
        if (req != 1 || res != RESULT_OK) return
        val uri = data?.data ?: return
        val dst = File(filesDir, "in.apk")
        contentResolver.openInputStream(uri)?.use { inp ->
            dst.outputStream().use { out -> inp.copyTo(out) }
        }
        val outDir = (getExternalFilesDir("Dex") ?: filesDir).absolutePath
        Thread {
            Native.run(dst.absolutePath, outDir)
            dst.delete()
        }.start()
    }
}
