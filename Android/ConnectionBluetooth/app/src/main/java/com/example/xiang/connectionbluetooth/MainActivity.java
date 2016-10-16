package com.example.xiang.connectionbluetooth;

import android.app.Activity;

import android.os.Handler;
import android.os.Message;
import android.os.Bundle;

import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import android.util.Log;

public class MainActivity extends Activity {
    private static final String TAG = "ＭainActivity";
    // 設定 Bluetooth MAC Address
    private final String address = "98:D3:31:40:25:86";
    // 初始 BluetoothThread
    BluetoothThread btt;
    // 藍牙連線訊息處理
    Handler writeHandler;

    public void connectButtonPressed(View v) {
        // Log 按下連線按鈕
        Log.v(TAG, "Connect button pressed.");
        // Log 藍牙已經連線
        if (btt != null) {
            Log.w(TAG, "Already connected!");
            return;
        }

        // 初始藍牙執行緒
        btt = new BluetoothThread(address, new Handler() {
            @Override
            public void handleMessage(Message message) {
                String s = (String) message.obj;
                // 藍牙連線狀態
                if (s.equals("CONNECTED")) {
                    TextView tv = (TextView) findViewById(R.id.statusText);
                    tv.setText("Connected.");
                    Button b = (Button) findViewById(R.id.writeButton);
                    b.setEnabled(true);
                } else if (s.equals("DISCONNECTED")) {
                    TextView tv = (TextView) findViewById(R.id.statusText);
                    Button b = (Button) findViewById(R.id.writeButton);
                    b.setEnabled(false);
                    tv.setText("Disconnected.");
                } else if (s.equals("CONNECTION FAILED")) {
                    TextView tv = (TextView) findViewById(R.id.statusText);
                    tv.setText("Connection failed!");
                    btt = null;
                } else {
                    TextView tv = (TextView) findViewById(R.id.readField);
                    tv.setText(s);
                }
            }
        });

        // Get the handler that is used to send messages
        writeHandler = btt.getWriteHandler();
        // Run the thread
        btt.start();
        TextView tv = (TextView) findViewById(R.id.statusText);
        tv.setText("Connecting...");
    }

    // 取消藍牙連線
    public void disconnectButtonPressed(View v) {
        Log.v(TAG, "Disconnect button pressed.");

        if(btt != null) {
            btt.interrupt();
            btt = null;
        }
    }

    /**
     * Send a message using the Bluetooth thread's write handler.
     */
    public void writeButtonPressed(View v) {
        Log.v(TAG, "Write button pressed.");

        TextView tv = (TextView)findViewById(R.id.writeField);
        String data = tv.getText().toString();

        Message msg = Message.obtain();
        msg.obj = data;
        writeHandler.sendMessage(msg);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button b = (Button) findViewById(R.id.writeButton);
        b.setEnabled(false);
    }

    /**
     * Kill the thread when we leave the activity.
     */
    protected void onPause() {
        super.onPause();

        if(btt != null) {
            btt.interrupt();
            btt = null;
        }
    }
}