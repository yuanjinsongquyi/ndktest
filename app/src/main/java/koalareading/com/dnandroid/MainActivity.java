package koalareading.com.dnandroid;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.SurfaceView;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {


    private SurfaceView surfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        surfaceView = findViewById(R.id.view);
//        final DNPlayer player = new DNPlayer();
//        player.setSurfaceView(surfaceView);
//        player.setDataSource("rtmp://edge1.everyon.tv:1935/etv1sb/phd18");
//        player.prepare();
//        player.setOnPrepareListener(new DNPlayer.OnPrepareListener() {
//            @Override
//            public void onPrepare() {
//                runOnUiThread(new Runnable() {
//                    @Override
//                    public void run() {
//                        TextView textView=(TextView)findViewById(R.id.sample_text);
//                        textView.setText("haole");
//
//                    }
//                });
//                player.start();
//            }
//        });

    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}
