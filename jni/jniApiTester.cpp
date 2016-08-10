#include <jni.h>
#include <android/log.h>
#include <string.h>

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOG_TAG "JNIApiTester"

extern "C" {

jmethodID someMethodID;
jfieldID someFieldID;

jboolean
Java_com_itsec_jniapitester_JniApiTester_testGetStringUTFChars(JNIEnv* env, jobject thiz, jstring s) {
	jboolean b = JNI_TRUE;
	const char* chars = env->GetStringUTFChars(s, &b);
	const char* expected = "someString";
	jboolean result = 0;
	if (strcmp(chars, expected) == 0) {
		LOGD("GetStringUTFChars: SUCCESS");
		result = JNI_TRUE;
	} else {
		LOGD("GetStringUTFChars: FAIL (%s != %s)", chars, expected);
		result = JNI_FALSE;
	}
	env->ReleaseStringUTFChars(s, chars);
	return result;
}

jstring
Java_com_itsec_jniapitester_JniApiTester_testNewStringUTF(JNIEnv* env, jobject thiz) {
	jstring result = env->NewStringUTF("someString");
	return result;
}

jboolean
Java_com_itsec_jniapitester_JniApiTester_testGetVersion(JNIEnv* env, jobject thiz) {
	jint result = env->GetVersion();
	if (result == JNI_VERSION_1_6) {
		LOGD("GetVersion: SUCCESS");
		return JNI_TRUE;
	} else
		LOGD("GetVersion: FAIL (%d != %d)", result, JNI_VERSION_1_6);
	return JNI_FALSE;
}

jclass
Java_com_itsec_jniapitester_JniApiTester_testFindClass(JNIEnv* env, jobject thiz) {
	jclass result = env->FindClass("android/app/Activity");
	LOGD("android.app.Activity=%08x", (int)result);
	return result;
}

jclass
Java_com_itsec_jniapitester_JniApiTester_testGetSuperclass(JNIEnv* env, jobject thiz, jclass clazz) {
	jclass result = env->GetSuperclass(clazz);
	LOGD("class %08x -> superclass %08x", (int)clazz, (int)result);
	return result;
}

jboolean
Java_com_itsec_jniapitester_JniApiTester_testIsAssignableFrom(JNIEnv* env, jobject thiz, jclass clazz1, jclass clazz2) {
	jboolean result = env->IsAssignableFrom(clazz1, clazz2);
	LOGD("%08x assignable from %08x: %08x", (int)clazz1, (int)clazz2, (int)result);
	return result;
}

jobject
Java_com_itsec_jniapitester_JniApiTester_testAllocObject(JNIEnv* env, jobject thiz, jclass clazz) {
	jobject result = env->AllocObject(clazz);
	return result;
}

jint
Java_com_itsec_jniapitester_JniApiTester_testThrow(JNIEnv* env, jobject thiz, jthrowable jt) {
	jint result = env->Throw(jt);
	return result;
}

jint
Java_com_itsec_jniapitester_JniApiTester_testThrowNew(JNIEnv* env, jobject thiz, jclass jclazz) {
	jint result = env->ThrowNew(jclazz, "Throw it high and wide");
	return result;
}

jthrowable
Java_com_itsec_jniapitester_JniApiTester_testExceptionOccurred(JNIEnv* env, jobject thiz, jclass jc) {
	jint e = env->ThrowNew(jc, "cheeky exception");
	if (e!=0) LOGE("Unexpected result of throwing exception: %d", e);
	jthrowable jt = env->ExceptionOccurred();
	env->ExceptionDescribe();
	env->ExceptionClear();
	env->ExceptionDescribe();
	return jt;
}

jboolean
Java_com_itsec_jniapitester_JniApiTester_testExceptionCheck(JNIEnv* env, jobject thiz, jclass jc) {
	jint e = env->ThrowNew(jc, "check me if you can");
	return env->ExceptionCheck();
}

void
Java_com_itsec_jniapitester_JniApiTester_testGetMethodID(JNIEnv* env, jobject thiz, jclass jc) {
	someMethodID = env->GetMethodID(jc, "callSomething", "(I)V");
}

void
Java_com_itsec_jniapitester_JniApiTester_testGetStaticMethodID(JNIEnv* env, jobject thiz, jclass jc) {
	someMethodID = env->GetStaticMethodID(jc, "printInt", "(I)V");
	env->CallStaticVoidMethod(jc, someMethodID, 568);
}

jshort
Java_com_itsec_jniapitester_JniApiTester_testCallStaticShortMethod(JNIEnv* env, jobject thiz, jclass jc) {
	someMethodID = env->GetStaticMethodID(jc, "staticNum", "(S)S");
	jshort param = 498;
	jshort result = env->CallStaticShortMethodA(jc, someMethodID, (jvalue*)&param);
	return result;
}


jint
Java_com_itsec_jniapitester_JniApiTester_testGetFieldID(JNIEnv* env, jobject thiz, jobject jobj) {
	jclass jc = env->GetObjectClass(jobj);
	someFieldID = env->GetFieldID(jc, "myField", "I");
	if (someFieldID == 0) return 0;
	jint secret = env->GetIntField(jobj, someFieldID);
	if (secret == 43) return 3;
	else return 0;
}

jint
Java_com_itsec_jniapitester_JniApiTester_testFromReflectedField(JNIEnv* env, jobject thiz, jobject jobj, jobject jfield) {
	someFieldID = env->FromReflectedField(jfield);
	if (someFieldID == 0) return 0;
	jint secret = env->GetIntField(jobj, someFieldID);
	if (secret == 43) return 1;
	else return 0;
}

jint
Java_com_itsec_jniapitester_JniApiTester_testGetObjectField(JNIEnv* env, jobject thiz, jobject jobj, jobject result) {
	jclass jc = env->GetObjectClass(jobj);
	someFieldID = env->GetFieldID(jc, "inception", "Lcom/itsec/jniapitester/TestObject;");
	if (someFieldID == 0) return 0;
	jobject inception = env->GetObjectField(jobj, someFieldID);
	if (result == inception) return 1;
	LOGD("FAIL GetObjectField: %08x != %08x", (int)result, (int)inception);
	return 0;
}

jint
Java_com_itsec_jniapitester_JniApiTester_testGetCharField(JNIEnv* env, jobject thiz, jobject jobj, jchar result) {
	jclass jc = env->GetObjectClass(jobj);	
	someFieldID = env->GetFieldID(jc, "charField", "C");
	if (someFieldID == 0) return 0;
	jchar charField = env->GetCharField(jobj, someFieldID);
	if (result == charField) return 1;
	LOGD("FAIL GetCharField: %c != %c", result, charField);
	return 0;
}

void
Java_com_itsec_jniapitester_JniApiTester_testCallSomethingByte(JNIEnv* env, jobject thiz, jclass jc) {
	someMethodID = env->GetMethodID(jc, "callSomethingByte", "()B");
	
}

jboolean
Java_com_itsec_jniapitester_JniApiTester_testCallSomethingBoolean(JNIEnv* env, jobject thiz, jobject jobj, jclass jc, jint i, jdouble d) {
	LOGD("now calling env->GetMethodID");
	someMethodID = env->GetMethodID(jc, "callSomethingInt", "()I");
	if (someMethodID == 0)
		return JNI_FALSE;
	LOGD("now calling env->CallIntMethod(jobj=%08x)", (int)jobj);
	jint ii = env->CallIntMethod(jobj, someMethodID);
	if (ii == 86) {
		LOGD("CallIntMethod: SUCCESS");
	} else LOGD("CallIntMethod: FAIL (unexpected result %d!=86)", ii);
	va_list list;
	LOGD("now calling env->CallIntMethodV(jobj=%08x)", (int)jobj);
	ii = env->CallIntMethodV(jobj, someMethodID, list);
	if (ii == 86) {
		LOGD("CallIntMethodV: SUCCESS");
	} else LOGD("CallIntMethodV: FAIL (unexpected result %d!=86)", ii);
	jvalue* args = (jvalue*)malloc(2*sizeof(jvalue));
	LOGD("now calling env->CallIntMethodA(jobj=%08x)", (int)jobj);
	ii = env->CallIntMethodA(jobj, someMethodID, args);
	if (ii == 86) {
		LOGD("CallIntMethodA: SUCCESS");
	} else LOGD("CallIntMethodA: FAIL (unexpected result %d!=86)", ii);

	someMethodID = env->GetMethodID(jc, "callSomethingBoolean", "(ID)Z");
	if (someMethodID == 0)
		return JNI_FALSE;
	LOGD("now calling env->CallBooleanMethod");
	jboolean result = env->CallBooleanMethod(jobj, someMethodID, i, d);
	args[0].i=i;
	args[1].d=d;
	LOGD("now calling env->CallBooleanMethodA");
	result = env->CallBooleanMethodA(jobj, someMethodID, args);
	return JNI_TRUE;
}

jboolean
Java_com_itsec_jniapitester_JniApiTester_testCallNonvirtual(JNIEnv* env, jobject thiz, jobject jobj, jclass jc) {
	LOGD("now calling env->CallNonvirtualObjectMethodA()");
	jvalue* args = 0;
	someMethodID = env->GetMethodID(jc, "callSomethingObject", "()Ljava/lang/Object;");
	if (someMethodID == 0) return JNI_FALSE;
	jobject o = env->CallNonvirtualObjectMethodA(jobj, jc, someMethodID, args);
	return JNI_TRUE;
}

jboolean
Java_com_itsec_jniapitester_JniApiTester_testIsInstanceOf(JNIEnv* env, jobject thiz, jobject jobj, jclass jc) {
	return env->IsInstanceOf(jobj, jc);
}

jboolean
Java_com_itsec_jniapitester_JniApiTester_testIsSameObject(JNIEnv* env, jobject thiz, jobject jobj1, jobject jobj2) {
	return env->IsSameObject(jobj1, jobj2);
}

jboolean
Java_com_itsec_jniapitester_JniApiTester_testSetShortArrayRegion(JNIEnv* env, jobject thiz, jshortArray jarr) {
	jshort* region = (jshort*)(malloc(3*sizeof(jshort)));
	region[0] = 23;
	region[1] = 29;
	region[2] = 31;
	env->SetShortArrayRegion(jarr, 4, 3, region);
	free(region);
	return JNI_TRUE;
}

jboolean
Java_com_itsec_jniapitester_JniApiTester_testGetByteArrayRegion(JNIEnv* env, jobject thiz, jbyteArray jarr) {
	jbyte* region = (jbyte*)(malloc(3*sizeof(jbyte)));
	env->GetByteArrayRegion(jarr, 4, 3, region);
	LOGD("Byte array region: %d, %d, %d", region[0], region[1], region[2]);
	free(region);
	return JNI_TRUE;
}

void Java_com_itsec_jniapitester_JniApiTester_testGetStringChars(JNIEnv* env, jobject thiz, jstring jstr) {
	jboolean b = JNI_FALSE;
	const jchar* chars = env->GetStringChars(jstr, &b);
	LOGD("received %s", (const char*)chars);
	//chars[2]='3';
	env->ReleaseStringChars(jstr, chars);
}

jboolean Java_com_itsec_jniapitester_JniApiTester_testNewObject(JNIEnv* env, jobject thiz, jclass jc) {
	jmethodID methodID = env->GetMethodID(jc, "<init>", "(I)V");
	LOGD("constructor methodID=%08x", (int)methodID);
	jobject newOne = env->NewObject(jc, methodID, 49);
	return JNI_TRUE;
}

void Java_com_itsec_jniapitester_JniApiTester_testArrayCritical(JNIEnv* env, jobject thiz, jshortArray jsa) {
	jboolean b = JNI_TRUE;
	LOGD("calling GetPrimitiveArrayCritical with jarr=%08x boolean=%08x", (int)jsa, (int)(&b));
	jshort* shorts = (jshort*)(env->GetPrimitiveArrayCritical(jsa, &b));
	LOGD("received data=%08x", (int)shorts);
	LOGD("shorts[0]=%d, shorts[1]=%d", shorts[0], shorts[1]);
	shorts[1] = 333;
	env->ReleasePrimitiveArrayCritical(jsa, shorts, 2);
}


}
