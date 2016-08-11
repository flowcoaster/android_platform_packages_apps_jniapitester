LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_LDFLAGS 	:= -llog
LOCAL_MODULE    := libjniapitester
LOCAL_SRC_FILES := jniApiTester.cpp

include $(BUILD_SHARED_LIBRARY)
