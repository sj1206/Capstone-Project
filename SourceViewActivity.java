package caucse.myapplication;

import android.content.Intent;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;
import android.widget.ListView;

public class SourceViewActivity extends AppCompatActivity {

    int imageName;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_source_view);

        ImageView iv = (ImageView) findViewById(R.id.imageView);
        imageName = getIntent().getIntExtra("imageName", 1);
        if (imageName == 1) {
            iv.setImageDrawable(ContextCompat.getDrawable(this, R.drawable.a1));
        }
        else if (imageName == 2) {
            iv.setImageDrawable(ContextCompat.getDrawable(this, R.drawable.a2));
        }
        else if (imageName == 3) {
            iv.setImageDrawable(ContextCompat.getDrawable(this, R.drawable.a3));
        }


        iv.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Intent intent = new Intent(SourceViewActivity.this, MainActivity.class);
                startActivity(intent);
            }
        });
    }
}
