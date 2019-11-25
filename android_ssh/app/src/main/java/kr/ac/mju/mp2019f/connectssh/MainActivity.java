package kr.ac.mju.mp2019f.connectssh;

import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.content.DialogInterface;
import android.content.pm.ActivityInfo;
import android.os.AsyncTask;
import android.os.Bundle;
import android.text.InputType;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.jcraft.jsch.Channel;
import com.jcraft.jsch.ChannelExec;
import com.jcraft.jsch.JSch;
import com.jcraft.jsch.JSchException;
import com.jcraft.jsch.Session;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.InterruptedIOException;
import java.util.Properties;

public class MainActivity extends AppCompatActivity {

    private TextView textView;
//    private String username = "spa152187";
//    private String host = "accs.mju.ac.kr";
//    private int port = 2018;
//    private String password = "rnrtk1203";
    private boolean flag = false;
    private String command = "./wri";

    private String username = "jeon";
    private String host = "211.169.221.73";
    private int port = 22;
    private String password = "xornjseh12";

    private Session session = null;
    private Channel channel = null;
    private ChannelExec channelExec = null;
    private JSch jsch = null;
    private Button button;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

        textView = findViewById(R.id.textView);
        jsch = new JSch();
        button = findViewById(R.id.button);

        getSupportActionBar().setDisplayOptions(ActionBar.DISPLAY_SHOW_CUSTOM);
        getSupportActionBar().setCustomView(R.layout.abs_layout);

        button.setOnLongClickListener(new View.OnLongClickListener(){
            @Override
            public boolean onLongClick(View view) {
                AlertDialog.Builder builder;
                if(flag) {
                    builder = command_dialog();
                }else {
                    builder = connect_dialog();
                }

                AlertDialog dialog = builder.create();
                dialog.show();

                return false;
            }
        });
    }

    private AlertDialog.Builder command_dialog() {
        LayoutInflater inflater = (LayoutInflater)getApplicationContext().getSystemService(LAYOUT_INFLATER_SERVICE);
        View v = inflater.inflate(R.layout.command_layout, null);

        final EditText et = v.findViewById(R.id.et_com);

        AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
        builder.setTitle("커맨드 입력");       // 제목 설정
        builder.setMessage("커맨드를 입력해주세요");   // 내용 설정
        builder.setView(v);
        builder.setPositiveButton("Yes", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                Log.d("yes", "Yes Btn Click");

                // Text 값 받아서 로그 남기기
                command = et.getText().toString();
                dialog.dismiss();     //닫기
                // Event
            }
        });

        builder.setNegativeButton("No", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                Log.d("no","No Btn Click");
                dialog.dismiss();     //닫기
            }
        });
        return builder;
    }

    private AlertDialog.Builder connect_dialog() {
        LayoutInflater inflater = (LayoutInflater)getApplicationContext().getSystemService(LAYOUT_INFLATER_SERVICE);
        View v = inflater.inflate(R.layout.connect_layout, null);

        final EditText et = v.findViewById(R.id.et_ip);
        final EditText et2 = v.findViewById(R.id.et_name);
        final EditText et3 = v.findViewById(R.id.et_pass);

        AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
        builder.setTitle("정보입력");       // 제목 설정
        builder.setMessage("IP,아이디,비밀번호를 입력해주세요");   // 내용 설정
        builder.setView(v);
        builder.setPositiveButton("Yes", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                Log.d("yes", "Yes Btn Click");

                // Text 값 받아서 로그 남기기
                host = et.getText().toString();
                port = 22;
                if(et2.getText().toString() != "")
                    username = et2.getText().toString();
                if(et3.getText().toString() != "")
                    password = et3.getText().toString();

                dialog.dismiss();     //닫기
                // Event
            }
        });

        builder.setNegativeButton("No", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                Log.d("no","No Btn Click");
                dialog.dismiss();     //닫기
            }
        });
        return builder;
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        if (channel != null && channel.isConnected()) {
            channel.disconnect();
        }
        if (session != null && session.isConnected()) {
            session.disconnect();
        }
    }

    public void click(View view) throws JSchException {

        new AsyncTask<Integer, Void, Void>(){
            @SuppressLint("WrongThread")
            @Override
            protected Void doInBackground(Integer... params) {
                try {
                    System.out.println("==> Connecting to " + host);
                    try {
                        //세션에 접속하기위한 사전준비

                        session = jsch.getSession(username, host, port);
                        session.setPassword(password);

                        Properties config = new Properties();
                        config.put("StrictHostKeyChecking", "no");
                        session.setConfig(config);
                        //세션에 접속
                        session.connect();
                        Log.d("session","==> Connected to " + host);

                        channel = session.openChannel("exec");
                        channelExec = (ChannelExec) channel;

                        Log.d("channel","==> Connected to " + host);

                        InputStream in = channelExec.getInputStream();
                        //커맨드라인에 보낼 메시지.

                        if(flag){
                            channelExec.setCommand(command);
                            channelExec.connect();
                        }else{
                            channelExec.setCommand("echo GOOD");
                            channelExec.connect();
                        }

                        Log.d("channelExec","==> Connected to " + host);

                        BufferedReader reader = new BufferedReader(new InputStreamReader(in));
                        String line;
                        textView.setText("");
                        while ((line = reader.readLine()) != null)
                        {
                            if(flag){
                                textView.append(line + "\n");
                            }else{
                                if(line.equals("GOOD")) {
                                    flag = true;
                                    button.setText("SEND COMMAND");

                                    textView.setText("connect complete");
                                }
                            }
                        }
                        if(flag && textView.getText() == ""){
                            textView.setText("return message is null OR\n send wrong message");
                        }

                    } catch (JSchException e) {
                        e.printStackTrace();
                        Log.d("error1",e.toString());
                        textView.setText("connect fail");
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                    Log.d("error2",e.toString());
                    textView.setText("connect fail");
                }
                return null;
            }
        }.execute(1);
    }
}
