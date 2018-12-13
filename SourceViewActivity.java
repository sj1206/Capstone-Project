package com.example.caucse.db0602;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.ImageView;

import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;

public class SourceViewActivity extends AppCompatActivity {


    String imageName;
    Bitmap bmImg1;
    Bitmap bmImg2;
    Bitmap bmImg3;
    Bitmap bmImg4;
    ImageView imView;
    int image_cnt = 0;
    String imgUrl = "http://teamssdweb.kr/Cam_image/";
    back task;
    back2 task2;
    back3 task3;
    back4 task4;
    ImageView iv1,iv2,iv3,iv4;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_source_view);
        for(int i=0;i<4;i++) {
            if(i==0) {
                imageName = getIntent().getStringExtra("imageName1");
                task = new back();
                task.execute(imgUrl + 15 + ".jpg");
            }
            else if(i==1) {
                imageName = getIntent().getStringExtra("imageName2");
                task2 = new back2();
                task2.execute(imgUrl + 14 + ".jpg");
            }
            else if(i==2) {
                imageName = getIntent().getStringExtra("imageName3");
                task3 = new back3();
                task3.execute(imgUrl + 16 + ".jpg");
            }
            else if(i==3) {
                imageName = getIntent().getStringExtra("imageName4");
                task4 = new back4();
                task4.execute(imgUrl + 17 + ".jpg");
            }
        }
        iv1 = findViewById(R.id.imageView2);
        iv2 = findViewById(R.id.imageView3);
        iv3 = findViewById(R.id.imageView4);
        iv4 = findViewById(R.id.imageView5);

        iv1.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                finish();
            }
        });
        iv2.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                finish();
            }
        });
        iv3.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                finish();
            }
        });
        iv4.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                finish();
            }
        });


    }
    private class back extends AsyncTask<String, Integer,Bitmap>{

        @Override
        protected Bitmap doInBackground(String... urls) {
            // TODO Auto-generated method stub
            try{
                //  ArrayList<String> str =new ArrayList<String>();
                //StringTokenizer tokens = new StringTokenizer(urls.toString(),"*");
                URL myFileUrl = new URL(urls[0]);
                HttpURLConnection conn = (HttpURLConnection)myFileUrl.openConnection();
                conn.setDoInput(true);
                conn.connect();
                InputStream is = conn.getInputStream();

                    bmImg1 = BitmapFactory.decodeStream(is);

                // num = Integer.parseInt(tokens.nextToken());
            }catch(IOException e){
                e.printStackTrace();
            }
            return bmImg1;
        }
        protected void onPostExecute(Bitmap img){
            iv1.setImageBitmap(bmImg1);
        }

    }
    private class back2 extends AsyncTask<String, Integer,Bitmap>{

        @Override
        protected Bitmap doInBackground(String... urls) {
            // TODO Auto-generated method stub
            try{
                //  ArrayList<String> str =new ArrayList<String>();
                //StringTokenizer tokens = new StringTokenizer(urls.toString(),"*");
                URL myFileUrl = new URL(urls[0]);
                HttpURLConnection conn = (HttpURLConnection)myFileUrl.openConnection();
                conn.setDoInput(true);
                conn.connect();
                InputStream is = conn.getInputStream();

                bmImg2 = BitmapFactory.decodeStream(is);

                // num = Integer.parseInt(tokens.nextToken());
            }catch(IOException e){
                e.printStackTrace();
            }
            return bmImg2;
        }
        protected void onPostExecute(Bitmap img){
            iv2.setImageBitmap(bmImg2);
        }

    }
    private class back3 extends AsyncTask<String, Integer,Bitmap>{

        @Override
        protected Bitmap doInBackground(String... urls) {
            // TODO Auto-generated method stub
            try{
                //  ArrayList<String> str =new ArrayList<String>();
                //StringTokenizer tokens = new StringTokenizer(urls.toString(),"*");
                URL myFileUrl = new URL(urls[0]);
                HttpURLConnection conn = (HttpURLConnection)myFileUrl.openConnection();
                conn.setDoInput(true);
                conn.connect();
                InputStream is = conn.getInputStream();

                bmImg3 = BitmapFactory.decodeStream(is);

                // num = Integer.parseInt(tokens.nextToken());
            }catch(IOException e){
                e.printStackTrace();
            }
            return bmImg3;
        }
        protected void onPostExecute(Bitmap img){
            iv3.setImageBitmap(bmImg3);
        }

    }
    private class back4 extends AsyncTask<String, Integer,Bitmap>{

        @Override
        protected Bitmap doInBackground(String... urls) {
            // TODO Auto-generated method stub
            try{
                //  ArrayList<String> str =new ArrayList<String>();
                //StringTokenizer tokens = new StringTokenizer(urls.toString(),"*");
                URL myFileUrl = new URL(urls[0]);
                HttpURLConnection conn = (HttpURLConnection)myFileUrl.openConnection();
                conn.setDoInput(true);
                conn.connect();
                InputStream is = conn.getInputStream();

                bmImg4 = BitmapFactory.decodeStream(is);

                // num = Integer.parseInt(tokens.nextToken());
            }catch(IOException e){
                e.printStackTrace();
            }
            return bmImg4;
        }

        protected void onPostExecute(Bitmap img){
            iv4.setImageBitmap(bmImg4);
        }

    }

}