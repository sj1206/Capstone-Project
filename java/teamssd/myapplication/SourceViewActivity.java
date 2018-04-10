package teamssd.myapplication;

import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.ImageView;

public class SourceViewActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_source_view);

        ImageView iv = (ImageView) findViewById(R.id.imageView);
        iv.setImageDrawable(ContextCompat.getDrawable(this, R.drawable.a1));
    }
}
