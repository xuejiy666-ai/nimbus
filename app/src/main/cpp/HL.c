JNIEXPORT jobjectArray JNICALL Java_com_mint_toolbox_Native_run(JNIEnv *env, jclass cls,
                                                                 jstring apk, jstring out)
{
    (void) cls;
    const char *apkPath = (*env)->GetStringUTFChars(env, apk, NULL);
    const char *outDir = (*env)->GetStringUTFChars(env, out, NULL);
    UnpackResult r;
    memset(&r, 0, sizeof(r));
    int ok = sc_unpack_run(apkPath ? apkPath : "", outDir ? outDir : "", &r);
    if (apkPath) (*env)->ReleaseStringUTFChars(env, apk, apkPath);
    if (outDir) (*env)->ReleaseStringUTFChars(env, out, outDir);

    jclass strCls = (*env)->FindClass(env, "java/lang/String");
    jobjectArray arr = (*env)->NewObjectArray(env, 3, strCls, NULL);
    jstring s0 = (*env)->NewStringUTF(env, ok ? "1" : "0");
    jstring s1 = (*env)->NewStringUTF(env, r.outDir ? r.outDir : "");
    jstring s2 = (*env)->NewStringUTF(env, r.pkg ? r.pkg : "");
    (*env)->SetObjectArrayElement(env, arr, 0, s0);
    (*env)->SetObjectArrayElement(env, arr, 1, s1);
    (*env)->SetObjectArrayElement(env, arr, 2, s2);
    (*env)->DeleteLocalRef(env, s0);
    (*env)->DeleteLocalRef(env, s1);
    (*env)->DeleteLocalRef(env, s2);
    sc_unpack_result_free(&r);
    return arr;
}
