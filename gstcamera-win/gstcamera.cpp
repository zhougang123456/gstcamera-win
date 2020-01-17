#include "gstcamera.h"
#include "gst/gst.h"
#include <gst/video/gstvideometa.h>
#include "stdio.h"

static gboolean gst_camera_init() {

	static int success = 0;
	if (!success) {
		GError* err = NULL;
		if (gst_init_check(NULL, NULL, &err)) {
			success = 1;
		}
		else {
			g_warning("Disabling GStreamer video support: %s", err->message);
			g_clear_error(&err);
			success = -1;
		}
	}
	return success > 0;
}

void create_pipeline(HWND handle)
{	

	if (!gst_camera_init()) {
		return;
	}

	gchar* desc =
		g_strdup_printf("ksvideosrc ! glimagesink name=sink");
	
	GError* err = NULL;
	printf("GStreamer pipeline: %s", desc);

	GstElement* pipeline = gst_parse_launch_full(desc, NULL, GST_PARSE_FLAG_FATAL_ERRORS, &err);
	
	GstVideoOverlay* overlay = GST_VIDEO_OVERLAY(gst_bin_get_by_name(GST_BIN(pipeline), "sink"));
	gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(overlay), (guintptr)handle);
	gst_video_overlay_expose(GST_VIDEO_OVERLAY(overlay));
	g_free(desc);

	if (!pipeline || err) {
		
		g_clear_error(&err);
		return;
	}

	if (gst_element_set_state(pipeline, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
		return;
	}
}
