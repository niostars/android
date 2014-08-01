package com.nova.novapro;

import android.app.Activity;
import android.app.ProgressDialog;
import android.os.Bundle;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends Activity {

	static{
		System.loadLibrary("jpeg2bmp");
	}
	
	TextView tv_test = null;
	
	public native String StringFromC();
	public native int Jpeg2Bmp();
	public native int TransImage();
	private Button btn_convert = null;
	private Button btn_transition = null;
	private ProgressDialog pd;
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        
        
        tv_test = (TextView) this.findViewById(R.id.tv_test);
        btn_convert = (Button) this.findViewById(R.id.btn_convert);
        btn_transition = (Button) this.findViewById(R.id.btn_transition);
        
        
        btn_convert.setOnClickListener(new OnConverClistListener());
        btn_transition.setOnClickListener(new OnTransitionClistListener());
    }


    //图像转换监听器
    private class OnConverClistListener implements OnClickListener{

		@Override
		public void onClick(View arg0) {
			//显示进度条对话框
			pd = new ProgressDialog(MainActivity.this);
			pd.setTitle("提示");
			pd.setMessage("正在转码....");
			pd.setMax(100);
			pd.setProgressStyle(ProgressDialog.STYLE_SPINNER);
			pd.show();
			new Thread(new Runnable() {
				
				@Override
				public void run() {
					// TODO Auto-generated method stub
					int ret = Jpeg2Bmp();
					if(ret == 0){
						pd.dismiss();
						//tv_test.setText(StringFromC());
					}
				}
			}).start();
		}
    	
    }
    
    
  //ͼ���?ť������
    private class OnTransitionClistListener implements OnClickListener{

		@Override
		public void onClick(View arg0) {
			//图像处理对话框
			pd = new ProgressDialog(MainActivity.this);
			pd.setTitle("提示ʾ");
			pd.setMessage("正在处理图片....");
			pd.setMax(100);
			pd.setProgressStyle(ProgressDialog.STYLE_SPINNER);
			pd.show();
			
			new Thread(new Runnable() {
				
				@Override
				public void run() {
					int ret = TransImage();
					if(ret == 0){
						pd.dismiss();
					}
					
				}
			}).start();
			
			
		}
    	
    }
    
    
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }
    
}
