package com.mint.toolbox

object Native {
    init {
        System.loadLibrary("app")
    }

    external fun run(apk: String, out: String): Array<String>
}
