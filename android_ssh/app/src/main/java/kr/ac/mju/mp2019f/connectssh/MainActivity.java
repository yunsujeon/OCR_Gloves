package kr.ac.mju.mp2019f.connectssh;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.content.DialogInterface;
import android.content.pm.ActivityInfo;
import android.os.AsyncTask;
import android.os.Bundle;
import android.text.InputType;
import android.util.Log;
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
    private String username = "spa152187";
    private String host = "accs.mju.ac.kr";
    private int port = 2018;
    private String password = "rnrtk1203";

//    private String username = "jeon";
//    private String host = "192.168.0.12";
//    private int port = 22;
//    private String password = "xornjseh12";

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

        button.setOnLongClickListener(new View.OnLongClickListener() {
            @Override
            public boolean onLongClick(View view) {
                AlertDialog.Builder ad = new AlertDialog.Builder(MainActivity.this);

                ad.setTitle("정보입력");       // 제목 설정
                ad.setMessage("IP,포트,아이디,비밀번호를 입력해주세요");   // 내용 설정

                // EditText 삽입하기

                final EditText et = new EditText(MainActivity.this);
                final EditText et1 = new EditText(MainActivity.this);
                final EditText et2 = new EditText(MainActivity.this);
                final EditText et3 = new EditText(MainActivity.this);
                et1.setInputType(InputType.TYPE_CLASS_NUMBER);
                LinearLayout lay = new LinearLayout(MainActivity.this);
                lay.setOrientation(LinearLayout.VERTICAL);
                lay.addView(et);
                lay.addView(et1);
                lay.addView(et2);
                lay.addView(et3);
                ad.setView(lay);

                // 확인 버튼 설정
                ad.setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Log.d("yes", "Yes Btn Click");

                        // Text 값 받아서 로그 남기기
                        host = et.getText().toString();
                        port = Integer.parseInt(et1.getText().toString());
                        username = et2.getText().toString();
                        password = et3.getText().toString();

                        dialog.dismiss();     //닫기
                        // Event
                    }
                });
                // 취소 버튼 설정
                ad.setNegativeButton("No", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Log.d("no","No Btn Click");
                        dialog.dismiss();     //닫기
                    }
                });

                // 창 띄우기
                ad.show();

                return false;
            }
        });
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
                        //해당 메시지(cat ./asdf/text.txt) 를 채널로 보냄
                        channelExec.setCommand("echo GOOD");
                        channelExec.connect();


                        Log.d("channelExec","==> Connected to " + host);

                        BufferedReader reader = new BufferedReader(new InputStreamReader(in));
                        String line;
                        textView.setText("");

                        while ((line = reader.readLine()) != null)
                        {
                            textView.append(line + "\n");
                        }

                    } catch (JSchException e) {
                        e.printStackTrace();
                        Log.d("error1",e.toString());
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                    Log.d("error2",e.toString());
                }
                return null;
            }
        }.execute(1);
    }
}
