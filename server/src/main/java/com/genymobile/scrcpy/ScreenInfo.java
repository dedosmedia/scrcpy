package com.genymobile.scrcpy;

import android.graphics.Rect;

public final class ScreenInfo {
    private final Rect contentRect; // device size, possibly cropped
    private final Size deviceSize;  // device size, not cropped
    private final Size videoSize;
    private final boolean rotated;
    private int rotation;

    public ScreenInfo(Rect contentRect, Size deviceSize, Size videoSize, boolean rotated, int rotation) {
        this.deviceSize = deviceSize;
        this.contentRect = contentRect;
        this.videoSize = videoSize;
        this.rotated = rotated;
        this.rotation = rotation;
    }

    public int getRotation() {
        return rotation;
    }

    public Size getDeviceSize() {
        return deviceSize;
    }

    public Rect getContentRect() {
        return contentRect;
    }

    public Size getVideoSize() {
        return videoSize;
    }

    public ScreenInfo withRotation(int rotation) {

        boolean newRotated = (rotation & 1) != 0;

        if (rotated == newRotated) {
            this.rotation = rotation;  // change rotation value when device just rotate from ROTATION_90 to ROTATION_270 or the other way around
            return this;
        }

        return new ScreenInfo(Device.flipRect(contentRect), deviceSize.rotate(), videoSize.rotate(), newRotated, rotation);
    }
}
