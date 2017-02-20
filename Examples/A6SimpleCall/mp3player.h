#ifndef MP3PLAYER_H
#define MP3PLAYER_H
#define MAX_VOLUME 30
class MP3Player
{
  public:
    enum eCommands {NEXT_SONG=1,PREV_SONG,PLAY_INDEX,VOL_UP,VOL_DOWN,SET_VOL,SINGLE_CYCLE_FILE=8,SELECT_DEV,SLEEP_MODE,
           WAKE_UP, RESET, RESUME_PLAY,PAUSE_PLAY,PLAY_FOLDER_NAME,STOP_PLAY=22,CYCLE_FOLDER,SINGLE_CYCLE=25,
           SET_DAC,PLAY_VOLUME=34,DISK_LOADED=0x3A,DISK_EJECT,UTRACK_FINISHED,TTRACK_FINISHED,FEEDBACK_ERROR=0x40,FEEDBACK_REPLY,
           TRACKS_PER_FOLDER=0x4E,FOLDERS_PER_DISK};
    enum eError {NO_ERROR,SLEEPING=2,SERIAL_ERROR,RANGE=5,NOT_FOUND,PROTOCOL=8};
    MP3Player(Stream &comm);  // constructor
    ~MP3Player();
    void begin();
    void begin(bool);
    void PlayFolderFile(int folder,int file);
    void PlayNextInFolder();
    void SetVolume(unsigned int value);
    void VolumeUp();
    void VolumeDown();
    void Pause();
    void Resume();
    void Next();
    void Previous();
    void FeedbackCallback(byte[])  __attribute__((weak));
    void FoldersPerDiskQuery();
    void TracksPerFolderQuery(unsigned int);
    void FeedRead();
private:
    Stream *_comm3;
    enum eParse{WAIT_HDR,WAIT_VER,WAIT_LENGTH,GET_DATA};
    void SetCommand(eCommands command, uint16_t data);
    struct sCmdBlock {
      byte hdr,ver;
      byte length;
      byte cmd;
      byte feedback;
      uint16_t data;
      byte trl;
    } CmdBlock;
    byte currentVolume,currentFolder,currentFile;
    enum eParse parseState;
    byte feebackPayload[10];  // should be enough
    int feedbackIndex,feedbackLength;
    void analyseFeedback();
    void SendToDevice(byte[]);
    void Feedback(byte);
};
#endif

