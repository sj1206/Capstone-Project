package caucse.myapplication;


import android.content.Intent;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ListView;

import java.util.ArrayList;


public class MainActivity extends AppCompatActivity {

    ListView listview ;
    ListViewAdapter listAdapter;
    ArrayList<ListViewItem> listViewItemList;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        listview = (ListView) findViewById(R.id.listview1);
        listViewItemList = new ArrayList<ListViewItem>();

        listViewItemList.add(new ListViewItem(ContextCompat.getDrawable(this, R.drawable.a1), "Image_1", "2018-04-10 오후 03:42\n\nCAM1"));
        listViewItemList.add(new ListViewItem(ContextCompat.getDrawable(this, R.drawable.a2), "Image_2", "2018-04-08 오후 11:17\n\nCAM2"));
        listViewItemList.add(new ListViewItem(ContextCompat.getDrawable(this, R.drawable.a3), "Image_3", "2018-04-08 오전 09:55\n\nCAM1"));

        listAdapter = new ListViewAdapter(MainActivity.this, listViewItemList);
        listview.setAdapter(listAdapter);

        listview.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int position, long l) {
                Intent zoomintent = new Intent(MainActivity.this,SourceViewActivity.class);
                zoomintent.putExtra("imageName", position + 1);
                startActivity(zoomintent);
            }
        });
    }


}