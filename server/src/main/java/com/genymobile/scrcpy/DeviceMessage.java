package com.genymobile.scrcpy;

public final class DeviceMessage {

    public static final int TYPE_CLIPBOARD = 0;
    public static final int TYPE_SCREENSHOT = 1;

    private int type;
    private String text;

    private DeviceMessage() {
    }

    public static DeviceMessage createMessage(int type, String text) {
        DeviceMessage event = new DeviceMessage();
        event.type = type;
        event.text = text;
        return event;
    }

    public int getType() {
        return type;
    }

    public String getText() {
        return text;
    }
}
