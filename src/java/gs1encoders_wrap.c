/*
 * JNI interface for accessing the GS1 Syntax Engine native library from Java
 * using the org.gs1.gs1encoders.GS1Encoder wrapper class.
 *
 * Copyright (c) 2022 GS1 AISBL.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <jni.h>
#include <stdio.h>
#include "gs1encoders.h"


#if defined(__clang__)
#  define DIAG_PUSH _Pragma("clang diagnostic push")
#  define DIAG_POP _Pragma("clang diagnostic pop")
#  define DIAG_DISABLE_DEPRECATED_DECLARATIONS _Pragma("clang diagnostic ignored \"-Wdeprecated-declarations\"")
#elif defined(__GNUC__)
#  define DIAG_PUSH _Pragma("GCC diagnostic push")
#  define DIAG_POP _Pragma("GCC diagnostic pop")
#  define DIAG_DISABLE_DEPRECATED_DECLARATIONS _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#elif defined(_MSC_VER)
#  define DIAG_PUSH __pragma(warning(push))
#  define DIAG_POP __pragma(warning(pop))
#  define DIAG_DISABLE_DEPRECATED_DECLARATIONS __pragma(warning(disable: 4996))
#endif


JNIEXPORT jstring JNICALL Java_org_gs1_gs1encoders_GS1Encoder_gs1encoderGetErrMsgJNI(
        JNIEnv* env,
        jobject obj,
        jlong ctx) {
    char *out = gs1_encoder_getErrMsg((gs1_encoder*)ctx);
    return (*env)->NewStringUTF(env, out);
}

JNIEXPORT jlong JNICALL Java_org_gs1_gs1encoders_GS1Encoder_gs1encoderInitJNI(
        JNIEnv* env,
        jobject obj) {
    return (jlong)gs1_encoder_init(NULL);
}

JNIEXPORT void JNICALL Java_org_gs1_gs1encoders_GS1Encoder_gs1encoderFreeJNI(
        JNIEnv* env,
        jobject obj,
        jlong ctx) {
    gs1_encoder_free((gs1_encoder*)ctx);
}

JNIEXPORT jstring JNICALL Java_org_gs1_gs1encoders_GS1Encoder_gs1encoderGetVersionJNI(
        JNIEnv* env,
        jobject obj) {
    char *out = gs1_encoder_getVersion();
    return (*env)->NewStringUTF(env, out);
}

JNIEXPORT jint JNICALL Java_org_gs1_gs1encoders_GS1Encoder_gs1encoderGetSymJNI(
        JNIEnv* env,
        jobject obj,
        jlong ctx) {
    return gs1_encoder_getSym((gs1_encoder*)ctx);
}

JNIEXPORT jboolean JNICALL Java_org_gs1_gs1encoders_GS1Encoder_gs1encoderSetSymJNI(
        JNIEnv* env,
        jobject obj,
        jlong ctx,
        jint sym) {
    return gs1_encoder_setSym((gs1_encoder*)ctx, sym);
}

JNIEXPORT jboolean JNICALL Java_org_gs1_gs1encoders_GS1Encoder_gs1encoderGetAddCheckDigitJNI(
        JNIEnv* env,
        jobject obj,
        jlong ctx) {
    return gs1_encoder_getAddCheckDigit((gs1_encoder*)ctx);
}

JNIEXPORT jboolean JNICALL Java_org_gs1_gs1encoders_GS1Encoder_gs1encoderSetAddCheckDigitJNI(
        JNIEnv* env,
        jobject obj,
        jlong ctx,
        jboolean value) {
    return gs1_encoder_setAddCheckDigit((gs1_encoder*)ctx, value);
}

JNIEXPORT jboolean JNICALL Java_org_gs1_gs1encoders_GS1Encoder_gs1encoderGetIncludeDataTitlesInHRIJNI(
        JNIEnv* env,
        jobject obj,
        jlong ctx) {
    return gs1_encoder_getIncludeDataTitlesInHRI((gs1_encoder*)ctx);
}

JNIEXPORT jboolean JNICALL Java_org_gs1_gs1encoders_GS1Encoder_gs1encoderSetIncludeDataTitlesInHRIJNI(
        JNIEnv* env,
        jobject obj,
        jlong ctx,
        jboolean value) {
    return gs1_encoder_setIncludeDataTitlesInHRI((gs1_encoder*)ctx, value);
}

JNIEXPORT jboolean JNICALL Java_org_gs1_gs1encoders_GS1Encoder_gs1encoderGetPermitUnknownAIsJNI(
        JNIEnv* env,
        jobject obj,
        jlong ctx) {
    return gs1_encoder_getPermitUnknownAIs((gs1_encoder*)ctx);
}

JNIEXPORT jboolean JNICALL Java_org_gs1_gs1encoders_GS1Encoder_gs1encoderSetPermitUnknownAIsJNI(
        JNIEnv* env,
        jobject obj,
        jlong ctx,
        jboolean value) {
    return gs1_encoder_setPermitUnknownAIs((gs1_encoder*)ctx, value);
}

JNIEXPORT jboolean JNICALL Java_org_gs1_gs1encoders_GS1Encoder_gs1encoderGetValidationEnabledJNI(
        JNIEnv* env,
        jobject obj,
        jlong ctx,
        jint validation) {
    return gs1_encoder_getValidationEnabled((gs1_encoder*)ctx, (enum gs1_encoder_validations)validation);
}

JNIEXPORT jboolean JNICALL Java_org_gs1_gs1encoders_GS1Encoder_gs1encoderSetValidationEnabledJNI(
        JNIEnv* env,
        jobject obj,
        jlong ctx,
        jint validation,
        jboolean value) {
    return gs1_encoder_setValidationEnabled((gs1_encoder*)ctx, (enum gs1_encoder_validations)validation, value);
}

JNIEXPORT jboolean JNICALL Java_org_gs1_gs1encoders_GS1Encoder_gs1encoderGetValidateAIassociationsJNI(
        JNIEnv* env,
        jobject obj,
        jlong ctx) {
DIAG_PUSH
DIAG_DISABLE_DEPRECATED_DECLARATIONS
    return gs1_encoder_getValidateAIassociations((gs1_encoder*)ctx);
DIAG_POP
}

JNIEXPORT jboolean JNICALL Java_org_gs1_gs1encoders_GS1Encoder_gs1encoderSetValidateAIassociationsJNI(
        JNIEnv* env,
        jobject obj,
        jlong ctx,
        jboolean value) {
DIAG_PUSH
DIAG_DISABLE_DEPRECATED_DECLARATIONS
    return gs1_encoder_setValidateAIassociations((gs1_encoder*)ctx, value);
DIAG_POP
}

JNIEXPORT jstring JNICALL Java_org_gs1_gs1encoders_GS1Encoder_gs1encoderGetDataStrJNI(
        JNIEnv* env,
        jobject obj,
        jlong ctx) {
    char *out = gs1_encoder_getDataStr((gs1_encoder*)ctx);
    return (*env)->NewStringUTF(env, out);
}

JNIEXPORT jboolean JNICALL Java_org_gs1_gs1encoders_GS1Encoder_gs1encoderSetDataStrJNI(
        JNIEnv* env,
        jobject obj,
        jlong ctx,
        jstring value) {
    const char* str;
    jboolean isCopy;
    jboolean ret;

    str = (*env)->GetStringUTFChars(env, value, &isCopy);
    ret = gs1_encoder_setDataStr((gs1_encoder*)ctx, str);

    if (isCopy == JNI_TRUE)
        (*env)->ReleaseStringUTFChars(env, value, str);

    return ret;
}

JNIEXPORT jstring JNICALL Java_org_gs1_gs1encoders_GS1Encoder_gs1encoderGetAIdataStrJNI(
        JNIEnv* env,
        jobject obj,
        jlong ctx) {
    char *out = gs1_encoder_getAIdataStr((gs1_encoder*)ctx);
    return (*env)->NewStringUTF(env, out);
}

JNIEXPORT jboolean JNICALL Java_org_gs1_gs1encoders_GS1Encoder_gs1encoderSetAIdataStrJNI(
        JNIEnv* env,
        jobject obj,
        jlong ctx,
        jstring value) {
    const char* str;
    jboolean isCopy;
    jboolean ret;

    str = (*env)->GetStringUTFChars(env, value, &isCopy);
    ret = gs1_encoder_setAIdataStr((gs1_encoder*)ctx, str);

    if (isCopy == JNI_TRUE)
        (*env)->ReleaseStringUTFChars(env, value, str);

    return ret;
}

JNIEXPORT jstring JNICALL Java_org_gs1_gs1encoders_GS1Encoder_gs1encoderGetScanDataJNI(
        JNIEnv* env,
        jobject obj,
        jlong ctx) {
    char *out = gs1_encoder_getScanData((gs1_encoder*)ctx);
    return (*env)->NewStringUTF(env, out);
}

JNIEXPORT jboolean JNICALL Java_org_gs1_gs1encoders_GS1Encoder_gs1encoderSetScanDataJNI(
        JNIEnv* env,
        jobject obj,
        jlong ctx,
        jstring value) {
    const char* str;
    jboolean isCopy;
    jboolean ret;

    str = (*env)->GetStringUTFChars(env, value, &isCopy);
    ret = gs1_encoder_setScanData((gs1_encoder*)ctx, str);

    if (isCopy == JNI_TRUE)
        (*env)->ReleaseStringUTFChars(env, value, str);

    return ret;
}

JNIEXPORT jstring JNICALL Java_org_gs1_gs1encoders_GS1Encoder_gs1encoderGetErrMarkupJNI(
        JNIEnv* env,
        jobject obj,
        jlong ctx) {
    char *out = gs1_encoder_getErrMarkup((gs1_encoder*)ctx);
    return (*env)->NewStringUTF(env, out);
}

JNIEXPORT jstring JNICALL Java_org_gs1_gs1encoders_GS1Encoder_gs1encoderGetDLuriJNI(
        JNIEnv* env,
        jobject obj,
        jlong ctx,
        jstring stem) {

    const char* out;
    const char* str;
    jboolean isCopy = JNI_FALSE;

    str = stem ? (*env)->GetStringUTFChars(env, stem, &isCopy) : NULL;
    out = gs1_encoder_getDLuri((gs1_encoder*)ctx, (char*)str);
    if (isCopy == JNI_TRUE)
        (*env)->ReleaseStringUTFChars(env, stem, str);

    return (*env)->NewStringUTF(env, out);
}

JNIEXPORT jobjectArray JNICALL Java_org_gs1_gs1encoders_GS1Encoder_gs1encoderGetHRIJNI(
        JNIEnv* env,
        jobject obj,
        jlong ctx) {
    char **hri;
    jobjectArray ret;
    int i, numAIs;
    numAIs = gs1_encoder_getHRI((gs1_encoder*)ctx, &hri);
    ret = (jobjectArray)(*env)->NewObjectArray(env, numAIs,
                                               (*env)->FindClass(env, "java/lang/String"),
                                               (*env)->NewStringUTF(env, ""));
    for (i = 0; i < numAIs; i++)
        (*env)->SetObjectArrayElement(env, ret, i, (*env)->NewStringUTF(env, hri[i]));
    return ret;
}

JNIEXPORT jobjectArray JNICALL Java_org_gs1_gs1encoders_GS1Encoder_gs1encoderGetDLignoredQueryParamsJNI(
        JNIEnv* env,
        jobject obj,
        jlong ctx) {
    char **qp;
    jobjectArray ret;
    int i, numAIs;
    numAIs = gs1_encoder_getDLignoredQueryParams((gs1_encoder*)ctx, &qp);
    ret = (jobjectArray)(*env)->NewObjectArray(env, numAIs,
                                               (*env)->FindClass(env, "java/lang/String"),
                                               (*env)->NewStringUTF(env, ""));
    for (i = 0; i < numAIs; i++)
        (*env)->SetObjectArrayElement(env, ret, i, (*env)->NewStringUTF(env, qp[i]));
    return ret;
}
