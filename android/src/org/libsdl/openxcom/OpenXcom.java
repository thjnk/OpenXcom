package org.libsdl.openxcom;

import java.util.Timer;
import java.util.TimerTask;

import org.libsdl.app.SDLActivity;
import org.libsdl.openxcom.UiVisibilityChanger;
//import org.libsdl.openxcom.DirsConfigActivity;

import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.content.pm.ActivityInfo;

/*
 * OpenXcom-specific additions to SDLActivity.
 *
 */


public class OpenXcom extends SDLActivity { 

	public static int systemUIStyle;

	protected final static String SYSTEM_UI_NAME = "SystemUIStyle";
	protected final int SYSTEM_UI_ALWAYS_SHOWN = 0;
	protected final int SYSTEM_UI_LOW_PROFILE = 1;
	protected final int SYSTEM_UI_IMMERSIVE = 2;

	protected UiVisibilityChanger uiVisibilityChanger = null;
	
	@Override
	protected void onCreate(Bundle savedInstance) {
		super.onCreate(savedInstance);
		SharedPreferences preferences = getSharedPreferences(DirsConfigActivity.PREFS_NAME, 0);
		systemUIStyle = preferences.getInt(SYSTEM_UI_NAME, 0);
		uiVisibilityChanger = new UiVisibilityChanger(this, systemUIStyle);
		setSystemUI();
	}
	
	@Override
	protected void onResume() {
		super.onResume();
		// Set UI a bit later, so that SDL would get the resize event
		Timer timer = new Timer();
		timer.schedule(new TimerTask() {
			@Override
			public void run() {
			setSystemUI();
			}
		}, 1000);
	}

	public void setSystemUI() {
		uiVisibilityChanger.setUiVisibilityFlags(systemUIStyle);
		runOnUiThread(uiVisibilityChanger);			
	}

	public static void showDirDialog() {
		android.app.Activity context = (android.app.Activity) SDLActivity.getContext();
		Log.i("OpenXcom", "Starting directory configuration dialog...");
		Intent intent = new Intent(context, DirsConfigActivity.class);
		context.startActivityForResult(intent, 0);
	}

	public void changeSystemUI(int newSystemUIStyle) {
		Log.i("OpenXcom", "Changing system UI");
		Log.i("OpenXcom", "New style is: " + newSystemUIStyle);
		systemUIStyle = newSystemUIStyle;
		SharedPreferences preferences = getSharedPreferences(DirsConfigActivity.PREFS_NAME, 0);
		SharedPreferences.Editor preferencesEditor = preferences.edit();
		preferencesEditor.putInt(SYSTEM_UI_NAME, systemUIStyle);
		preferencesEditor.apply();
		setSystemUI();
	}
	
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);
		SharedPreferences preferences = getSharedPreferences(DirsConfigActivity.PREFS_NAME, 0);
		String dataPath = preferences.getString(DirsConfigActivity.DATA_PATH_KEY, "");
		String savePath = preferences.getString(DirsConfigActivity.SAVE_PATH_KEY, "");
		String confPath = preferences.getString(DirsConfigActivity.CONF_PATH_KEY, "");
		nativeSetPaths(dataPath, savePath, confPath);
	}
	
	public static native void nativeSetPaths(String dataPath, String savePath, String confPath);

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		if (newConfig.orientation == Configuration.ORIENTATION_PORTRAIT) {
			newConfig.orientation = Configuration.ORIENTATION_LANDSCAPE;
		}

		super.onConfigurationChanged(newConfig);
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE);
	}

};



