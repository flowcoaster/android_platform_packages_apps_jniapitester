LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_REQUIRED_MODULES := libjniapitester

LOCAL_PACKAGE_NAME := JniApiTester
LOCAL_CERTIFICATE := platform

include $(call all-makefiles-under, jni)

include $(BUILD_PACKAGE)
