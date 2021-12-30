package de.kai_morich.simple_bluetooth_le_terminal;

import android.os.Debug;
import android.text.SpannableStringBuilder;
import android.util.Log;
import android.widget.TextView;

import java.lang.reflect.Array;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

public class WaveRemoteSocket {

    public static String byteArrayToHexaString(byte[] bytes) {
        StringBuilder builder = new StringBuilder();
        for (byte data : bytes) {
            builder.append(String.format("%02X ", data));
        }
        return builder.toString();
    }

    private class SettingValuesClass {
        Byte Lv = 0;
        Byte Mode = 0;
        Boolean MainPower = false;
        Boolean AutoNext = true;
        Boolean AutoOff = true;
        Boolean MuteMode = true;
        Integer AutoNextTimer = 0;
        Integer AutoOffTimer = 0;
        Float FirmwareVersion = 0.f;

        public void SetDataBytes(byte[] datas) {
//            typedef struct {
//                uint16_t AutoNextTimer : 12;
//                uint8_t Lv : 4;
//                uint16_t AutoOffTimer : 12;
//                uint8_t Mode : 3;
//                bool MainPower : 1;
//                bool AutoNext : 1;
//                bool AutoOff : 1;
//                bool MuteMode : 1;
//            }SettingValues;

            AutoNextTimer = (((datas[0])                //                NNNN NNNN
                    | (datas[1] << 8))                  //      NNNN NNNN 0000 0000
                    & 0x0FFF);                          //           1111 1111 1111 = 0x0FFF

            Lv = ((byte) ((datas[1] & 0xF0) >>> 4));    // 1111 0000 = 0xF0

            AutoOffTimer = (((datas[2])                 //                NNNN NNNN
                    | (datas[3] << 8))                  //      NNNN NNNN 0000 0000
                    & 0x0FFF);                          //           1111 1111 1111 = 0x0FFF

            Mode = ((byte) ((datas[3] & 0x70) >>> 4));  // 0111 0000 = 0x70

            MainPower = (0 != (datas[3] & 0x80));       // 1000 0000 = 0x80
            AutoNext = (0 != (datas[4] & 0x01));        // 0000 0001 = 0x01
            AutoOff = (0 != (datas[4] & 0x02));         // 0000 0010 = 0x02
            MuteMode = (0 != (datas[4] & 0x04));        // 0000 0100 = 0x04


        }

        public void SetDataFirmVer(byte[] datas) {
            FirmwareVersion = ByteBuffer.wrap(datas).order(ByteOrder.LITTLE_ENDIAN).getFloat();
        }
    }

    public class ArrayListCustom<E> extends ArrayList<E> {
        public void removeRange(int fromIndex, int toIndex) {
            super.removeRange(fromIndex, toIndex);
        }
    }

    private static final String TAG = "WaveRemoteSocket";
    private SettingValuesClass m_SetVal = new SettingValuesClass();
    public ArrayListCustom<Byte> m_Datas = new ArrayListCustom<Byte>();
    private StringBuilder LogText;
    private TextView receiveText;

//    private static WaveRemoteSocket m_WaveRemoteSocket;
//    public static WaveRemoteSocket instance() {
//        if (m_WaveRemoteSocket == null) {
//            m_WaveRemoteSocket = new WaveRemoteSocket();
//        }
//        return m_WaveRemoteSocket;
//    }

    public WaveRemoteSocket(TextView SetreceiveText) {
        receiveText = SetreceiveText;
        LogText = new StringBuilder();
    }

    public void PutData(byte Data) {
        m_Datas.add(Data);
        CheckProtocol();
    }

    int i = 0;

    public void PutData(byte[] Datas) {
        for (int i = 0; i < Datas.length; i++) {
            m_Datas.add(Datas[i]);
        }
        CheckProtocol();
    }

    private int m_Datas_CheckId = 0;
    static final byte[] m_Protocol_Common = {0x55, 0x0A, 0x00, (byte) 0xFF};

    public enum P_Type {
        C_CheckConnect,
        D_CheckConnect,

        C_InitProcess,
        D_InitProcess,

        C_StopProcess,
        D_StopProcess,

        C_StartProcess,
        D_StartProcess,

        C_CheckDataSize,
        D_CheckDataSize,

        C_SendDatas,
        D_SendDatas,

        C_SendGetVersion,
        D_SendGetVersion,

        C_EchoStatus,
        D_EchoStatus,

        C_Remote_Stop,
        D_Remote_Stop,

        C_Remote_LvUp,
        D_Remote_LvUp,

        C_Remote_LvDw,
        D_Remote_LvDw,

        C_Remote_ModeUp,
        D_Remote_ModeUp,

        C_Remote_ModeDw,
        D_Remote_ModeDw,

        C_Remote_MainPower,
        D_Remote_MainPower,

        C_Remote_AutoNext,
        D_Remote_AutoNext,

        C_Remote_AutoOff,
        D_Remote_AutoOff,

        C_Remote_MuteMode,
        D_Remote_MuteMode,

        P_Type_End;
    }

    static final byte m_Protocol[] = {
            0x00,        //C_CheckConnect
            0x50,        //D_CheckConnect

            0x01,        //C_InitProcess
            0x51,        //D_InitProcess

            0x02,        //C_StopProcess
            0x52,        //D_StopProcess

            0x03,        //C_StartProcess
            0x53,        //D_StartProcess

            0x04,        //C_CheckDataSize
            0x54,        //D_CheckDataSize

            0x05,        //C_SendDatas
            0x55,        //D_SendDatas

            0x06,        //C_SendGetVersion
            0x56,        //D_SendGetVersion

            0x07,        //C_EchoStatus
            0x57,        //D_EchoStatus

            0x20,        //C_Remote_Stop
            0x70,        //D_Remote_Stop

            0x21,        //C_Remote_LvUp
            0x71,        //D_Remote_LvUp

            0x22,        //C_Remote_LvDw
            0x72,        //D_Remote_LvDw

            0x23,        //C_Remote_ModeUp
            0x73,        //D_Remote_ModeUp

            0x24,        //C_Remote_ModeDw
            0x74,        //D_Remote_ModeDw

            0x25,        //C_Remote_MainPower
            0x75,        //D_Remote_MainPower

            0x26,        //C_Remote_AutoNext
            0x76,        //D_Remote_AutoNext

            0x27,        //C_Remote_AutoOff
            0x77,        //D_Remote_AutoOff

            0x28,        //C_Remote_MuteMode
            0x78,        //D_Remote_MuteMode
    };

    private enum CheckProcess_Type {
        CP_Normal,
        CP_EchoStatus,
        CP_SendGetVersion,
        CP_CheckDataSize,
        CP_SendDatas,
    }

    private CheckProcess_Type m_CheckProcess = CheckProcess_Type.CP_Normal;

    private void CheckProtocol() {
        try {
            while (true) {
                byte[] tmp_Datas;

                switch (m_CheckProcess) {

                    case CP_Normal: {
                        // 기본 프로토콜 확인

                        // 확인할 데이터가 부족한지 확인
                        if (m_Datas.size() < (m_Datas_CheckId + 4)) {
                            return;
                        }

                        // 확인할 배열 복사
                        tmp_Datas = new byte[4];
                        for (int i = 0; i < tmp_Datas.length; i++) {
                            tmp_Datas[i] = m_Datas.get(i + m_Datas_CheckId);
                        }

                        // 다음 체크할 위치 업데이트
                        ++m_Datas_CheckId;

                        // 기본 프로토콜 형태인지 확인
                        if (tmp_Datas[0] != m_Protocol_Common[0]
                                || tmp_Datas[1] != m_Protocol_Common[1]
                                //|| tmp_Datas[2] != m_Protocol_Common[2]
                                || tmp_Datas[3] != m_Protocol_Common[3]) {
                            break;
                        }

                        P_Type ProtocolID = P_Type.P_Type_End;
                        for (int i = 0; i < m_Protocol.length; i++) {
                            if (m_Protocol[i] == tmp_Datas[2]) {
                                ProtocolID = P_Type.values()[i];
                                break;
                            }
                        }

                        // 부합되는 프로토콜이 아닌경우 종료
                        if (ProtocolID == P_Type.P_Type_End)
                            break;

                        // 확인된 위치까지 버퍼 삭제
                        m_Datas.removeRange(0, m_Datas_CheckId + tmp_Datas.length - 1);
                        m_Datas_CheckId = 0;

                        // 로그에 기록
                        AddLogProtocol(ProtocolID);

                        // 프로토콜 종류에 따라 추가 기능 발동
                        switch (ProtocolID) {

                            case D_EchoStatus: {
                                m_CheckProcess = CheckProcess_Type.CP_EchoStatus;
                                break;
                            }

                            case D_SendGetVersion: {
                                m_CheckProcess = CheckProcess_Type.CP_SendGetVersion;
                                break;
                            }

                            case D_CheckDataSize: {
                                m_CheckProcess = CheckProcess_Type.CP_CheckDataSize;
                                break;
                            }

                            case D_SendDatas: {
                                m_CheckProcess = CheckProcess_Type.CP_SendDatas;
                                break;
                            }

//                            case C_CheckConnect:
//                            case D_CheckConnect:
//                            case C_InitProcess:
//                            case D_InitProcess:
//                            case C_StopProcess:
//                            case D_StopProcess:
//                            case C_StartProcess:
//                            case D_StartProcess:
//                            case C_CheckDataSize:
//                            case C_SendDatas:
//                            case C_SendGetVersion:
//                            case C_EchoStatus:
//                            case C_Remote_Stop:
//                            case D_Remote_Stop:
//                            case C_Remote_LvUp:
//                            case D_Remote_LvUp:
//                            case C_Remote_LvDw:
//                            case D_Remote_LvDw:
//                            case C_Remote_ModeUp:
//                            case D_Remote_ModeUp:
//                            case C_Remote_ModeDw:
//                            case D_Remote_ModeDw:
//                            case C_Remote_MainPower:
//                            case D_Remote_MainPower:
//                            case C_Remote_AutoNext:
//                            case D_Remote_AutoNext:
//                            case C_Remote_AutoOff:
//                            case D_Remote_AutoOff:
//                            case C_Remote_MuteMode:
//                            case D_Remote_MuteMode:
//                            default: {
//                                break;
//                            }
                        }

                        break;
                    }

                    case CP_EchoStatus: {
                        // 설정값 프로토콜 확인

                        // 확인할 데이터가 부족한지 확인
                        if (m_Datas.size() < (m_Datas_CheckId + 5)) {
                            return;
                        }

                        // 확인할 배열 복사
                        tmp_Datas = new byte[5];
                        for (int i = 0; i < tmp_Datas.length; i++) {
                            tmp_Datas[i] = m_Datas.get(i + m_Datas_CheckId);
                        }

                        // 확인된 위치까지 버퍼 삭제
                        m_Datas.removeRange(0, m_Datas_CheckId + tmp_Datas.length - 1);
                        m_Datas_CheckId = 0;

                        // 현재 설정값 업데이트
                        m_SetVal.SetDataBytes(tmp_Datas);
                        AddLogProtocol(m_CheckProcess);

                        // 기본 체크로 프로세스 변경
                        m_CheckProcess = CheckProcess_Type.CP_Normal;
                        break;
                    }

                    case CP_SendGetVersion: {
                        // 펌웨어 버전 프로토콜 확인

                        // 확인할 데이터가 부족한지 확인
                        if (m_Datas.size() < (m_Datas_CheckId + 4)) {
                            return;
                        }

                        // 확인할 배열 복사
                        tmp_Datas = new byte[4];
                        for (int i = 0; i < tmp_Datas.length; i++) {
                            tmp_Datas[i] = m_Datas.get(i + m_Datas_CheckId);
                        }

                        // 확인된 위치까지 버퍼 삭제
                        m_Datas.removeRange(0, m_Datas_CheckId + tmp_Datas.length - 1);
                        m_Datas_CheckId = 0;

                        // 현재 설정값 업데이트
                        m_SetVal.SetDataFirmVer(tmp_Datas);
                        AddLogProtocol(m_CheckProcess);

                        // 기본 체크로 프로세스 변경
                        m_CheckProcess = CheckProcess_Type.CP_Normal;
                        break;
                    }

                    case CP_CheckDataSize: {
                        // 데이터 크기 프로토콜 확인

                        // 확인할 데이터가 부족한지 확인
                        if (m_Datas.size() < (m_Datas_CheckId + 14)) {
                            return;
                        }

                        // 확인할 배열 복사
                        tmp_Datas = new byte[14];
                        for (int i = 0; i < tmp_Datas.length; i++) {
                            tmp_Datas[i] = m_Datas.get(i + m_Datas_CheckId);
                        }

                        // 확인된 위치까지 버퍼 삭제
                        m_Datas.removeRange(0, m_Datas_CheckId + tmp_Datas.length - 1);
                        m_Datas_CheckId = 0;

                        // 현재 설정값 업데이트
                        AddLogProtocol(m_CheckProcess, tmp_Datas);

                        // 기본 체크로 프로세스 변경
                        m_CheckProcess = CheckProcess_Type.CP_Normal;
                        break;

                    }


//                    case CP_SendDatas: {
//                        // 차후 구현
//                    }
                }


                // while 종료 체크
                {
                    boolean IsLoop = false;
                    IsLoop = m_Datas.size() <= m_Datas_CheckId;

                    if (IsLoop)
                        continue;
                }
                break;
            }
        } catch (Exception e) {
            Log.e(TAG, "CheckProtocol: " + e);
            e.printStackTrace();
        }
    }

    private void AddLogProtocol(P_Type ProtocolID) {
        AddLogProtocol(ProtocolID, null);

    }

    private void AddLogProtocol(P_Type ProtocolID, byte[] Datas) {

        // 시간 넣기
        AddLogNowTime();
        AddLogDividerChar();

        // 프로토콜 넣기
        AddLogStr(ProtocolID.name());
        AddLogDividerChar();

        // 배열 넣기
        if (Datas != null) {
            AddLogStr(byteArrayToHexaString(Datas));
            AddLogDividerChar();
        }

        // 라인 종료
        AddLogStrLine();
    }

    private void AddLogProtocol(CheckProcess_Type ProcessID) {
        AddLogProtocol(ProcessID, null);
    }

    private void AddLogProtocol(CheckProcess_Type ProcessID, byte[] Datas) {

        // 시간 넣기
        AddLogNowTime();
        AddLogDividerChar();

        // 프로토콜 넣기
        AddLogStr(ProcessID.name());
        AddLogDividerChar();

        switch (ProcessID) {

            case CP_EchoStatus: {

                // 셋팅 현황을 로그에 추가
                AddLogStr(m_SetVal.Lv.toString());
                AddLogDividerChar();
                AddLogStr(m_SetVal.Mode.toString());
                AddLogDividerChar();
                AddLogStr(m_SetVal.MainPower.toString());
                AddLogDividerChar();
                AddLogStr(m_SetVal.AutoNext.toString());
                AddLogDividerChar();
                AddLogStr(m_SetVal.AutoOff.toString());
                AddLogDividerChar();
                AddLogStr(m_SetVal.MuteMode.toString());
                AddLogDividerChar();
                AddLogStr(m_SetVal.AutoNextTimer.toString());
                AddLogDividerChar();
                AddLogStr(m_SetVal.AutoOffTimer.toString());
                AddLogDividerChar();

                break;
            }

            case CP_SendGetVersion: {

                // 펌웨어 버전을 로그에 추가
                AddLogStr(m_SetVal.FirmwareVersion.toString());
                AddLogDividerChar();

                break;
            }

//            case CP_CheckDataSize:{
//                break;
//            }
//
//            case CP_SendDatas:{
//                break;
//            }
//
//            default:
//            case CP_Normal: {
//                break;
//            }
        }

        // 배열 넣기
        if (Datas != null) {
            AddLogStr(byteArrayToHexaString(Datas));
            AddLogDividerChar();
        }

        // 라인 종료
        AddLogStrLine();
    }

    private void AddLogDividerChar() {
        LogText.append(',');
        receiveText.append(" ");
    }

    private void AddLogNowTime() {
        LogText.append(Long.toString(System.currentTimeMillis()));
        receiveText.append(
                (new SimpleDateFormat("HH:mm:ss.SS")).format(System.currentTimeMillis()));
    }

    private void AddLogStr(String str) {
        LogText.append(str);
        receiveText.append(str);
    }

    private void AddLogStrLine() {

        //A.append(System.lineSeparator());
        //A.append(System.getProperty("line.separator"));
        LogText.append(System.getProperty("line.separator"));
        receiveText.append(System.getProperty("line.separator"));
    }

    private void AddLogStrLine(String str) {
        AddLogStr(str);
        AddLogStrLine();
    }

    public void AddLogStatus(SpannableStringBuilder spn) {
        AddLogNowTime();
        AddLogDividerChar();
        LogText.append(spn);
        receiveText.append(spn);
        AddLogStrLine();
    }

    public byte[] SendDataMake(P_Type ProtocolID) {
        byte datas[] = m_Protocol_Common.clone();
        datas[2] = m_Protocol[ProtocolID.ordinal()];
        return datas;
    }

    public StringBuilder GetLogText()
    {
        return LogText;
    }


}
