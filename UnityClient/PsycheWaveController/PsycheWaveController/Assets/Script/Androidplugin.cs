using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Androidplugin : MonoBehaviour
{
    //private AndroidJavaObject UnityActivity;

    //private AndroidJavaObject UnityInstance;

    //void Start()
    //{
    //    AndroidJavaClass ajc = new AndroidJavaClass("com.unity3d.player.UnityPlayer");

    //    UnityActivity = ajc.GetStatic<AndroidJavaObject>("currentActivity");

    //    AndroidJavaClass ajc2 = new AndroidJavaClass("com.example.unitylove");
    //    UnityInstance = ajc2.CallStatic<AndroidJavaObject>("instance");

    //    UnityInstance.Call("setContext", UnityActivity);
    //}

    public void ToastButton()
    {
        ShowToast("테스트", false);
    }

    //public void ShowToast(string msg, bool isLong)
    //{
    //    UnityActivity.Call("runOnUiThread", new AndroidJavaRunnable(() =>
    //    {
    //        if(isLong == false)
    //        {
    //            UnityInstance.Call("ShowToast", msg, 0);
    //        }
    //        else
    //        {
    //            UnityInstance.Call("ShowToast", msg, 1);
    //        }
    //    }));
    //}

    private static AndroidJavaObject activityContext;
    private static AndroidJavaClass javaClass;
    private static AndroidJavaObject javaClassInstance;
    void Start()
    {
        using (AndroidJavaClass activityClass = new AndroidJavaClass("com.unity3d.player.UnityPlayer"))
        {
            activityContext = activityClass.GetStatic<AndroidJavaObject>("currentActivity");
        }
        using (javaClass = new AndroidJavaClass("com.empty.unitylove.Uplugin"))
        {
            if (javaClass != null)
            {
                javaClassInstance = javaClass.CallStatic<AndroidJavaObject>("instance");
                javaClassInstance.Call("setContext", activityContext);
            }
        }
    }

    //public static void CallShowToast(string text)
    //{
    //    activityContext.Call("runOnUiThread", new AndroidJavaRunnable(() => { javaClassInstance.Call("ShowToast", text); }));
    //}

    public void ShowToast(string msg, bool isLong)
    {
        //var testString = javaClassInstance.Call<string>("UnityCall", "testcall");

        activityContext.Call("runOnUiThread", new AndroidJavaRunnable(() =>
        {
            if (isLong == false)
            {
                javaClassInstance.Call("ShowToast", msg, 0);
            }
            else
            {
                javaClassInstance.Call("ShowToast", msg, 1);
            }
        }));
    }
}

