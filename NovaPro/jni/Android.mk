LOCAL_PATH := $(call my-dir)


## prebuilt the static libs
include $(CLEAR_VARS)
LOCAL_MODULE := jpeg
LOCAL_SRC_FILES := libjpeg.a
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE    := jpeg2bmp
LOCAL_SRC_FILES := jpeg2bmp.cpp convert.cpp saveDate2BMP.cpp locateimage.cpp
LOCAL_LDLIBS += -llog
LOCAL_STATIC_LIBRARIES += jpeg

include $(BUILD_SHARED_LIBRARY)
