package de.kai_morich.simple_bluetooth_le_terminal;

import android.util.Log;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;

public class FileIOSocket {

    private final static String TAG = "FileIOSocket";

    public static File FileSave(String FilePath, String Text) {
        try {
            File file = new File(FilePath);
            BufferedWriter BWriter = new BufferedWriter(new FileWriter(file, false));

            BWriter.append(Text);
            BWriter.close();

            return file;

        } catch (Exception e) {
            Log.e(TAG, "FileSave: " + e);
            e.printStackTrace();
        }
        return null;
    }
}
