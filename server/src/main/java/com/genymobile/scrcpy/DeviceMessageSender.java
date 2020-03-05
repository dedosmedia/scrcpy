package com.genymobile.scrcpy;

import java.io.IOException;

public final class DeviceMessageSender {

    private final DesktopConnection connection;

    private String message;
    private int type;

    public DeviceMessageSender(DesktopConnection connection) {
        this.connection = connection;
    }

    public synchronized void pushMessage(int _type, String text) {
        message = text;
        type = _type;
        notify();
    }

    public void loop() throws IOException, InterruptedException {
        while (true) {
            String text;
            synchronized (this) {
                while (message == null) {
                    wait();
                }
                text = message;
                message = null;
            }
            DeviceMessage event = DeviceMessage.createMessage(type, text);
            connection.sendDeviceMessage(event);
        }
    }
}
