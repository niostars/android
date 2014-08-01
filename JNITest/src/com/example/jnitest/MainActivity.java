package com.example.jnitest;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends Activity {

	static{
		System.loadLibrary("JNITest");
	}
	
	public native String StringFromC();
	
	private TextView tv_hello;
	private Button btn_hello;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		tv_hello = (TextView) this.findViewById(R.id.tv_hello);
		btn_hello = (Button) this.findViewById(R.id.btn_test);
		
		btn_hello.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
			
				tv_hello.setText(StringFromC());
				
			}
		});
		
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

}
