package de.kai_morich.simple_bluetooth_le_terminal;

import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.util.Log;

import androidx.core.content.FileProvider;

import java.io.File;

public class EmailSocket {

    private final static String TAG = "EmailSocket";

    public static void Send(
            Context context,
            String[] ReceiveAddress,
            String Subject,
            String EmailBody,
            File File) {
        try {

            Intent it = new Intent(Intent.ACTION_SEND);
            it.setType("plain/text");

            // 수신인 주소 - 배열의 값을 늘릴 경우 다수의 수신자에게 발송됨
            it.putExtra(Intent.EXTRA_EMAIL, ReceiveAddress);

            it.putExtra(Intent.EXTRA_SUBJECT, Subject);
            if (EmailBody != null)
                it.putExtra(Intent.EXTRA_TEXT, EmailBody);

            // 파일첨부
            if (File != null) {

                // 파일 첨부를 위해서는 Uri로 보내주어야 하며 7.0이상인 경우 FileProvider가 필요하다.
                Uri uri = null;
                if (Build.VERSION.SDK_INT > Build.VERSION_CODES.M) {
                    uri = FileProvider.getUriForFile(context, "de.kai_morich.simple_bluetooth_le_terminal.fileprovider", File);
                } else {
                    uri = Uri.fromFile(File);
                }

                it.putExtra(Intent.EXTRA_STREAM, uri);
            }

            context.startActivity(it.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK));


        } catch (Exception e) {
            Log.e(TAG, "Send: " + e);
            e.printStackTrace();
        }

    }
}
