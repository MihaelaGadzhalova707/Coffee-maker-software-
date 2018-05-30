package com.example.misheto.coffeemaker;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import java.io.IOException;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;

public class MainActivity extends AppCompatActivity {
    private TextView item;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button btnHit = (Button) findViewById(R.id.btnHit);
        Button btnCoffee1 = (Button) findViewById(R.id.coffee1);
        Button btnCoffee2 = (Button) findViewById(R.id.coffee2);
        item = (TextView) findViewById(R.id.JsonItem);
        btnHit.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                HttpURLConnection connection = null;
                try {
                    URL url = new URL("rety.dynu.net/on_off");
                    connection = (HttpURLConnection) url.openConnection();
                    connection.connect();
                    //item.setText("Button Clicked");
                }catch (MalformedURLException e) {
                    e.printStackTrace();
                }catch (IOException e) {
                    e.printStackTrace();
                }

            }
        });

        btnCoffee1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                HttpURLConnection connection = null;
                try {
                    URL url = new URL("rety.dynu.net/new_coffee/coffee_test?count=make_one_cup");
                    connection = (HttpURLConnection) url.openConnection();
                    connection.connect();
                    //item.setText("Button Clicked");
                }catch (MalformedURLException e) {
                    e.printStackTrace();
                }catch (IOException e) {
                    e.printStackTrace();
                }
            }
        });

        btnCoffee2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                HttpURLConnection connection = null;
                try {
                    URL url = new URL("rety.dynu.net/new_coffee/coffee_test?count=make_two_cups");
                    connection = (HttpURLConnection) url.openConnection();
                    connection.connect();
                    //item.setText("Button Clicked");
                }catch (MalformedURLException e) {
                    e.printStackTrace();
                }catch (IOException e) {
                    e.printStackTrace();
                }
            }
        });

    }
}
