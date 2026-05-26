#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <cmath>
#include <map>
#include <limits>
#include "SmartStudyPlanner.h"  //including header file
using namespace std;

#define RESET "\033[0m"
#define BOLD "\033[1m"
#define NEON_PURPLE "\033[38;5;135m"
#define NEON_PINK "\033[38;5;198m"
#define NEON_VIOLET "\033[38;5;93m"
#define NEON_CYAN "\033[38;5;51m"
#define NEON_GREEN "\033[38;5;82m"
#define NEON_LIME "\033[38;5;154m"
#define NEON_YELLOW "\033[38;5;226m"
#define NEON_ORANGE "\033[38;5;208m"
#define DIM_PURPLE "\033[38;5;61m"
#define DIM_CYAN "\033[38;5;24m"
#define DIM_WHITE "\033[38;5;252m"
#define BG_PURPLE "\033[48;5;54m"
#define BG_DARK "\033[48;5;17m"
#define RED NEON_PINK
#define GREEN NEON_GREEN
#define YELLOW NEON_YELLOW
#define BLUE NEON_PURPLE
#define CYAN NEON_CYAN

void cpk_line(char Fill, int Width) {
    cout<<DIM_PURPLE<<"_";
    for(int I=0; I<Width; I++)
        cout<<Fill;
    cout<<"_"<<RESET<<"\n";
}

void cpk_header(const string& Title) {
    cpk_line('_');
    cout<<DIM_PURPLE<<"| "<<RESET<<NEON_PURPLE<<BOLD<<left<<setw(43)<<Title<<RESET<<DIM_PURPLE<<"|"<<RESET<<"\n";
    cpk_line('_');
}

void cpk_section(const string& Label) {
    cout<<NEON_CYAN<<"\n__[ "<<RESET<<NEON_YELLOW<<BOLD<<Label<<RESET<<NEON_CYAN<<" ]";
    int Used=6+(int)Label.size()+2;
    for(int I=Used; I<45; I++)
        cout<<"_";
    cout<<"_"<<RESET<<"\n";
}

string neonBar(float Pct, int Width=16) {
    int Filled=(int)(Pct/100.0f*Width);
    string Bar=string(NEON_GREEN)+"[";
    for(int I=0; I<Filled; I++)
        Bar+="*";
    Bar+=string(DIM_PURPLE);
    for(int I=Filled; I<Width; I++)
        Bar+=".";
    Bar+=string(NEON_GREEN)+"]"+string(RESET);
    return Bar;
}

string priorityTag(float Pct) {
    if(Pct>=70)
        return string(NEON_GREEN)+" OK"+RESET;
    if(Pct>=40)
        return string(NEON_YELLOW)+" MED"+RESET;
    return string(NEON_PINK)+" LOW"+RESET;
}

struct Date {  // Simple Date structure with utility functions
    int Day, Month, Year;

    string StringConvert() const {
        ostringstream Out;
        Out<<Year<<"-"<<setw(2)<<setfill('0')<<Month<<"-"<<setw(2)<<setfill('0')<<Day;
        return Out.str();
    }

    static Date fromString(const string& S) {
        Date D;
        sscanf(S.c_str(), "%d-%d-%d", &D.Year, &D.Month, &D.Day);
        return D;
    }

    static Date GetTodayDate() {  // Gets the current date from the system
        time_t T=time(0);
        tm* Now=localtime(&T);
        Date D;
        D.Day=Now->tm_mday;
        D.Month=Now->tm_mon+1;
        D.Year=Now->tm_year+1900;
        return D;
    }

    int RemainingDays() const {  // Calculates the number of days remaining until this date from today
        tm DeadlineTm={};
        DeadlineTm.tm_mday=Day;
        DeadlineTm.tm_mon=Month-1;
        DeadlineTm.tm_year=Year-1900;
        time_t DeadlineT=mktime(&DeadlineTm);
        time_t NowT=time(0);
        double Diff=difftime(DeadlineT, NowT);
        return max(1, (int)(Diff/86400.0));
    }
};

class Task {  // Represents a study task with details and status
private:
    string TaskId, Topic;
    float EstMins, ActualMins;
    Date Deadline;
    bool IsDone, IsMissed;
    float Priority;

public:
    Task() : EstMins(60), ActualMins(0), IsDone(false), IsMissed(false), Priority(0) {
        Deadline=Date::GetTodayDate();
    }

    Task(string Id, string Top, float Mins, Date Dl): TaskId(Id), Topic(Top), EstMins(Mins), ActualMins(0), Deadline(Dl), IsDone(false), IsMissed(false), Priority(0) {}

    string getTaskId() const{ 
        return TaskId; }
    string getTopic() const{ 
        return Topic; }
    float getEstMins() const{ 
        return EstMins; }
    float getActualMins() const{ 
        return ActualMins; }
    Date getDeadline() const{ 
        return Deadline; }
    bool getIsDone() const{ 
        return IsDone; }
    bool getIsMissed() const{ 
        return IsMissed; }
    float getPriority() const{ 
        return Priority; }

    void setTaskId(string Id){ 
        TaskId=Id; }
    void setTopic(string T){ 
        Topic=T; }
    void setEstMins(float M){ 
        EstMins=M; }
    void setActualMins(float M){ 
        ActualMins=M; }
    void setDeadline(Date D){ 
        Deadline=D; }
    void setPriority(float P){ 
        Priority=P; }

    void markCompleted(){
        IsDone=true;
        cout<<NEON_GREEN<<"  Task \""<<Topic<<"\" marked as completed!"<<RESET<<"\n";
    }

    void markMissed(){
        IsMissed=true;
        cout<<NEON_PINK<<"  Task \""<<Topic<<"\" has been missed."<<RESET<<"\n";
    }

    bool operator>(const Task& Other) const {
        return Priority>Other.Priority;
    }

    void display() const {  // Displays the task details in a formatted manner with colored status                                      
        string Status;
        if(IsDone)
            Status=string(NEON_GREEN)+"[DONE]"+RESET;
        else if(IsMissed)
            Status=string(NEON_PINK)+"[MISSED]"+RESET;
        else
            Status=string(NEON_YELLOW)+"[PENDING]"+RESET;
        cout<<DIM_PURPLE<<"| "<<RESET<<NEON_CYAN<<left<<setw(9)<<TaskId<<DIM_WHITE<<setw(28)<<Topic<<NEON_YELLOW<<setw(10)<<(to_string((int)EstMins)+" min")<<DIM_WHITE<<setw(13)<<Deadline.StringConvert()<<Status<<DIM_PURPLE<<" |"<<RESET<<"\n";
    }

    string WriteFile() const {  // Converts the study session details into a comma-separated string for file storage
        return TaskId+","+Topic+","+to_string(EstMins)+","+to_string(ActualMins)+","+Deadline.StringConvert()+","+(IsDone?"1":"0")+","+(IsMissed?"1":"0");
    }

    static Task ReadFile(const string& Line) {
        stringstream Ss(Line);
        string Token;
        Task T;
        getline(Ss,Token,','); 
        T.TaskId=Token;
        getline(Ss,Token,','); 
        T.Topic=Token;
        getline(Ss,Token,','); 
        T.EstMins=stof(Token);
        getline(Ss,Token,','); 
        T.ActualMins=stof(Token);
        getline(Ss,Token,','); 
        T.Deadline=Date::fromString(Token);
        getline(Ss, Token,','); 
        T.IsDone=(Token=="1");
        getline(Ss,Token,','); 
        T.IsMissed=(Token=="1");
        return T;
    }
};

class StudySession {  //represent study session
private:
    string TaskId, SessDate;
    int PlannedMins, ActualMins;

public:
    StudySession() : PlannedMins(0), ActualMins(0) {}
    StudySession(string Tid, string D, int Planned, int Actual): TaskId(Tid), SessDate(D), PlannedMins(Planned), ActualMins(Actual) {}
    string getTaskId() const { 
        return TaskId; }
    string getDate() const { 
        return SessDate; }
    int getPlanned() const { 
        return PlannedMins; }
    int getActual() const { 
        return ActualMins; }

    void logSession() {  // Logs the study session details and prints a summary with feedback on goal achievement
        cpk_section("Session Logged");
        cout<<DIM_WHITE<<"  Task ID  : "<<NEON_CYAN<<TaskId<<RESET<<"\n";
        cout<<DIM_WHITE<<"  Date     : "<<NEON_PURPLE<<SessDate<<RESET<<"\n";
        cout<<DIM_WHITE<<"  Planned  : "<<NEON_YELLOW<<PlannedMins<<" min"<<RESET<<"\n";
        cout<<DIM_WHITE<<"  Actual   : "<<NEON_YELLOW<<ActualMins<<" min"<<RESET<<"\n";
        if(ActualMins>=PlannedMins)
            cout<<NEON_GREEN<<"  Goal met! Great work."<<RESET<<"\n";
        else
            cout<<NEON_YELLOW<<"  Under target, keep pushing!"<<RESET<<"\n";
        cpk_line();  // Print message if there are no urgent tasks
    }

    string WriteFile() const {  // Converts the study session details into a comma-separated string for file storage
        return TaskId+","+SessDate+","+to_string(PlannedMins)+","+to_string(ActualMins);
    }

    static StudySession ReadFile(const string& Line) {
        stringstream Ss(Line);
        string Tok;
        StudySession S;
        getline(Ss, Tok, ','); 
        S.TaskId=Tok;
        getline(Ss, Tok, ',');
        S.SessDate=Tok;
        getline(Ss, Tok, ','); 
        S.PlannedMins=stoi(Tok);
        getline(Ss, Tok, ','); 
        S.ActualMins=stoi(Tok);
        return S;
    }
};

class Subject {  // Represents a subject with its details and associated tasks
private:
    string Name, Deadline;
    int Diff, MissedPenalty;
    vector<Task> Tasks;

public:
    Subject() : Diff(1), MissedPenalty(0) {}
    Subject(string N, int D, string Dl, int Penalty=0): Name(N), Diff(D), Deadline(Dl), MissedPenalty(Penalty) {}

    string getName() const { 
        return Name; }
    int getDiff() const { 
        return Diff; }
    string getDeadline() const { 
        return Deadline; }
    int getPenalty() const { 
        return MissedPenalty; }
    vector<Task>& getTasks() { 
        return Tasks; }
    const vector<Task>& getTasks() const { 
        return Tasks; }

    void addTask(Task T) {
        Tasks.push_back(T);
        cout<<NEON_GREEN<<"  Task added to "<<NEON_CYAN<<Name<<RESET<<"\n";
    }

    void listTasks() const {
        if(Tasks.empty()) {
            cout<<NEON_YELLOW<<"  No tasks for "<<Name<<" yet.\n"<<RESET;
            return;
        }
        cpk_section("Tasks for "+Name);
        cout<<DIM_PURPLE<<"| "<<RESET<<NEON_PURPLE<<BOLD<<left<<setw(9)<<"ID"<<setw(28)<<"Topic"<<setw(10)<<"Duration"<<setw(13)<<"Deadline"<<"Status"<<RESET<<DIM_PURPLE<<" |"<<RESET<<"\n";
        cpk_line('=');  // Print header for today's plan
        for(const auto& T : Tasks)
            T.display();
        cpk_line();  // Print message if there are no urgent tasks
    }

    float calculatePriority() {
        Date Dl=Date::fromString(Deadline);
        int DaysLeft=Dl.RemainingDays();
        float Prio=(Diff*3.0f)+(100.0f/DaysLeft)+MissedPenalty;
        for(auto& T : Tasks)
            T.setPriority(Prio);
        return Prio;
    }

    float completionPercent() const {  // Calculates the completion percentage of tasks for the subject
        if(Tasks.empty())
            return 0.0f;
        int Done=0;
        for(const auto& T : Tasks)
            if(T.getIsDone())
                Done++;
        return (Done*100.0f)/Tasks.size();
    }

    void addMissedPenalty() { 
        MissedPenalty+=20; }

    string WriteFile() const {  // Converts the study session details into a comma-separated string for file storage
        return Name+","+to_string(Diff)+","+Deadline+","+to_string(MissedPenalty);
    }

    static Subject ReadFile(const string& Line) {
        stringstream Ss(Line);
        string Tok;
        Subject S;
        getline(Ss, Tok, ','); 
        S.Name=Tok;
        getline(Ss, Tok, ','); 
        S.Diff=stoi(Tok);
        getline(Ss, Tok, ','); 
        S.Deadline=Tok;
        getline(Ss, Tok, ','); 
        S.MissedPenalty=stoi(Tok);
        return S;
    }
};

class Notification {  //represents a notification with a message and urgency level
private:
    string Msg;
    int UrgencyLvl;

public:
    Notification(string M, int Lvl) : Msg(M), UrgencyLvl(Lvl) {}

    void trigger() const {
        if(UrgencyLvl==3)
            cout<<NEON_PINK<<BOLD<<"  [CRITICAL] "<<Msg<<RESET<<"\n";
        else if(UrgencyLvl==2)
            cout<<NEON_YELLOW<<"  [WARNING]  "<<Msg<<RESET<<"\n";
        else
            cout<<NEON_CYAN<<"  [INFO]     "<<Msg<<RESET<<"\n";
    }
};

class Analytics {  // Provides various analytics and insights based on subjects and study sessions
public:
    void subjectWiseProgress(vector<Subject>& Subjects) {
        cpk_section("Subject Progress");
        if(Subjects.empty()) {
            cout<<NEON_YELLOW<<"  No subjects added yet.\n"<<RESET;
            return;
        }
        cout<<"  "<<RESET<<NEON_PURPLE<<BOLD<<left<<setw(18)<<"Subject"<<setw(6)<<"Diff"<<setw(6)<<"Pct"<<"  Progress"<<RESET<<"\n";
        cpk_line('=');  // Print header for today's plan
        for(auto& S : Subjects) {
            float Pct=S.completionPercent();
            cout<<"  "<<NEON_CYAN<<left<<setw(18)<<S.getName()<<NEON_YELLOW<<setw(6)<<S.getDiff()<<DIM_WHITE<<setw(6)<<(to_string((int)Pct)+"%")<<"  "<<neonBar(Pct)<<priorityTag(Pct)<<"\n";
        }
        cpk_line();  // Print message if there are no urgent tasks
    }

    void weakSubjectDetector(vector<Subject>& Subjects) {
        if(Subjects.empty())
            return;
        Subject* Weakest=&Subjects[0];
        for(auto& S : Subjects)
            if(S.completionPercent()<Weakest->completionPercent())
                Weakest=&S;
        cpk_section("Weak Subject Alert");  // Displays the subject that whiich need most attention
        cout<<NEON_PINK<<BOLD<<"  \""<<Weakest->getName()<<"\" needs the most attention! ("<<(int)Weakest->completionPercent()<<"% complete)\n"<<RESET;
        cout<<DIM_WHITE<<"  Difficulty : "<<NEON_YELLOW<<Weakest->getDiff()<<RESET<<"   Deadline  : "<<NEON_PURPLE<<Weakest->getDeadline()<<RESET<<"\n";
        cpk_line();  // Print message if there are no urgent tasks
    }

    void studyHoursChart(vector<StudySession>& Sessions) {
        cpk_section("Study Hours Log");
        if(Sessions.empty()) {
            cout<<NEON_YELLOW<<"  No sessions logged yet.\n"<<RESET;
            return;
        }
        map<string,int> HoursMap;
        for(auto& S : Sessions)
            HoursMap[S.getDate()]+=S.getActual();
        for(auto& Entry : HoursMap) {
            int Hrs=Entry.second/60;
            int Mins=Entry.second%60;
            int Bars=Hrs+(Mins>30?1:0);
            cout<<"  "<<NEON_PURPLE<<left<<setw(13)<<Entry.first<<RESET<<" | "<<NEON_CYAN;
            for(int I=0; I<Bars; I++)
                cout<<"_";
            cout<<RESET<<"  "<<DIM_WHITE<<Hrs<<"h "<<Mins<<"m"<<RESET<<"\n";
        }
        cpk_line();  // Print message if there are no urgent tasks
    }

    void streakTracker(int CurStreak, int BestStreak) {
        cpk_section("Streak Tracker");
        cout<<"  "<<NEON_ORANGE<<"Current Streak : "<<NEON_GREEN<<CurStreak<<" day(s)"<<RESET<<"\n";
        cout<<"  "<<NEON_YELLOW<<"Longest Streak : "<<NEON_CYAN<<BestStreak<<" day(s)"<<RESET<<"\n";
        if(CurStreak>=7)
            cout<<NEON_GREEN<<"  Amazing! Unstoppable!"<<RESET<<"\n";
        else if(CurStreak>=3)
            cout<<NEON_YELLOW<<"  Good momentum, don't break it!"<<RESET<<"\n";
        else
            cout<<NEON_PINK<<"  Study daily to build your streak!"<<RESET<<"\n";
        cpk_line();  // Print message if there are no urgent tasks
    }

    void sessionComparison(vector<StudySession>& Sessions) {
        cpk_section("Session Comparison");
        if(Sessions.empty()) {
            cout<<NEON_YELLOW<<"  No sessions logged yet.\n"<<RESET;
            return;
        }
        cout<<"  "<<NEON_PURPLE<<BOLD<<left<<setw(11)<<"Task ID"<<setw(13)<<"Date"<<setw(11)<<"Planned"<<setw(11)<<"Actual"<<"Diff"<<RESET<<"\n";
        cpk_line('=');  // Print header for today's plan
        for(auto& S : Sessions) {
            int Diff=S.getActual()-S.getPlanned();
            cout<<"  "<<NEON_CYAN<<left<<setw(11)<<S.getTaskId()<<DIM_WHITE<<setw(13)<<S.getDate()<<NEON_YELLOW<<setw(11)<<(to_string(S.getPlanned())+" min")<<DIM_WHITE<<setw(11)<<(to_string(S.getActual())+" min");
            if(Diff>=0)
                cout<<NEON_GREEN<<"+"<<Diff<<" min";
            else
                cout<<NEON_PINK<<Diff<<" min";
            cout<<RESET<<"\n";
        }
        cpk_line();  // Print message if there are no urgent tasks
    }
};

class Scheduler {  //vreate the scheduale
private:
    vector<Task> AllTasks;

public:
    void loadTasks(vector<Subject>& Subjects) {
        AllTasks.clear();
        for(auto& S : Subjects) {
            S.calculatePriority();
            for(auto& T : S.getTasks())
                AllTasks.push_back(T);
        }
    }

    void sortTasksByPriority() {  //sort tasks by priority score in descending order
        sort(AllTasks.begin(), AllTasks.end(), [](const Task& A, const Task& B) {
            return A.getPriority()>B.getPriority();
        });
    }

    void generateSchedule() {
        sortTasksByPriority();
        cpk_section("Generated Study Schedule");  //displays the generated study schedule in a formatted manner with colored output
        if(AllTasks.empty()) {
            cout<<NEON_YELLOW<<"  No tasks to schedule.\n"<<RESET;
            return;
        }
        cout<<"  "<<NEON_PURPLE<<BOLD<<left<<setw(7)<<"Slot"<<setw(11)<<"Task ID"<<setw(26)<<"Topic"<<setw(10)<<"Duration"<<"Deadline"<<RESET<<"\n";
        cpk_line('=');  // Print header for today's plan
        int Slot=1;
        float TotalMins=0;
        for(auto& T : AllTasks) {
            if(T.getIsDone())
                continue;
            cout<<"  "<<NEON_PINK<<left<<setw(7)<<("#"+to_string(Slot++))<<NEON_CYAN<<setw(11)<<T.getTaskId()<<DIM_WHITE<<setw(26)<<T.getTopic()<<NEON_YELLOW<<setw(10)<<(to_string((int)T.getEstMins())+" min")<<NEON_PURPLE<<T.getDeadline().StringConvert()<<RESET<<"\n";
            TotalMins+=T.getEstMins();
        }
        cpk_line();  // Print message if there are no urgent tasks
        cout<<"  "<<DIM_WHITE<<"Total planned: "<<NEON_CYAN<<(int)(TotalMins/60)<<"h "<<(int)fmod(TotalMins,60)<<"m"<<RESET<<"\n";
        cpk_line();  // Print message if there are no urgent tasks
    }

    void rescheduleMissed(vector<Subject>& Subjects) {
        cpk_section("Rescheduling Missed Tasks");
        int Count=0;
        for(auto& S : Subjects) {
            for(auto& T : S.getTasks()) {
                if(T.getIsMissed()&&!T.getIsDone()) {
                    Date Dl=T.getDeadline();
                    Dl.Day+=1;
                    if(Dl.Day>30) { 
                        Dl.Day=1; Dl.Month++; }
                    if(Dl.Month>12) { 
                        Dl.Month=1; Dl.Year++; }
                    T.setDeadline(Dl);
                    S.addMissedPenalty();
                    cout<<NEON_YELLOW<<"  Rescheduled: \""<<NEON_CYAN<<T.getTopic()<<NEON_YELLOW<<"\" to "<<NEON_PURPLE<<Dl.StringConvert()<<RESET<<"\n";
                    Count++;
                }
            }
        }
        if(Count==0)
            cout<<NEON_GREEN<<"  No missed tasks to reschedule!\n"<<RESET;
        cpk_line();  // Print message if there are no urgent tasks
    }

    void flagUrgentTasks() {
        cpk_section("Urgent Task Alerts");
        bool Found=false;
        for(auto& T : AllTasks) {
            if(T.getIsDone())
                continue;
            int Days=T.getDeadline().RemainingDays();
            if(Days<=2) {
                Notification N("\""+T.getTopic()+"\" due in "+to_string(Days)+" day(s)! ["+T.getDeadline().StringConvert()+"]", Days==1?3:2);
                N.trigger();
                Found=true;
            }
        }
        if(!Found)
            cout<<NEON_GREEN<<"  No urgent tasks right now. Keep it up!\n"<<RESET;
        cpk_line();  // Print message if there are no urgent tasks
    }

    void printDailyPlan() {
        sortTasksByPriority();
        cpk_section("Today's Plan");
        cout<<"  "<<NEON_PURPLE<<BOLD<<left<<setw(11)<<"Task ID"<<setw(26)<<"Topic"<<setw(10)<<"Duration"<<"Deadline"<<RESET<<"\n";
        cpk_line('=');  // Print header for today's plan
        int Shown=0;
        float Total=0;
        for(auto& T : AllTasks) {
            if(T.getIsDone()||T.getIsMissed())
                continue;
            if(Shown>=5)
                break;
            cout<<"  "<<NEON_PINK<<"#"<<(Shown+1)<<" "<<NEON_CYAN<<left<<setw(9)<<T.getTaskId()<<DIM_WHITE<<setw(26)<<T.getTopic()<<NEON_YELLOW<<setw(10)<<(to_string((int)T.getEstMins())+" min")<<NEON_PURPLE<<T.getDeadline().StringConvert()<<RESET<<"\n";
            Total+=T.getEstMins();
            Shown++;
        }
        cpk_line();  // Print message if there are no urgent tasks
        if(Shown==0)
            cout<<NEON_GREEN<<"  All tasks done for today! Great work.\n"<<RESET;
        else
            cout<<"  "<<DIM_WHITE<<"Total: "<<NEON_CYAN<<(int)(Total/60)<<"h "<<(int)fmod(Total,60)<<"m across "<<NEON_PURPLE<<Shown<<" task(s)"<<RESET<<"\n";
        cpk_line();  // Print message if there are no urgent tasks
    }

    void printWeeklyPlan() {
        sortTasksByPriority();
        cpk_section("Weekly Overview (Next 7 Days)");
        Date Today=Date::GetTodayDate();
        for(int D=0; D<7; D++) {
            Date DayDate=Today;
            DayDate.Day+=D;
            if(DayDate.Day>30) { DayDate.Day-=30; DayDate.Month++; }
            if(DayDate.Month>12) { DayDate.Month=1; DayDate.Year++; }
            string DayStr=DayDate.StringConvert();
            bool HasTask=false;
            cout<<"\n  "<<NEON_CYAN<<"[ "<<DayStr<<" ]"<<RESET<<"\n";
            for(auto& T : AllTasks) {
                if(T.getIsDone())
                    continue;
                int Days=T.getDeadline().RemainingDays();
                if(Days==D) {
                    cout<<"    "<<NEON_PINK<<"-> "<<RESET<<DIM_WHITE<<T.getTopic()<<NEON_YELLOW<<" ("<<(int)T.getEstMins()<<" min)"<<RESET<<"\n";
                    HasTask=true;
                }
            }
            if(!HasTask)
                cout<<"    "<<DIM_PURPLE<<"(no tasks scheduled)"<<RESET<<"\n";
        }
        cout<<"\n";
        cpk_line();  // Print message if there are no urgent tasks
    }
};

class FileManager {
private:
    const string UsersFile="data/users.txt";
    const string SubjectsFile="data/subjects.txt";
    const string TasksFile="data/tasks.txt";
    const string SessionsFile="data/sessions.txt";

    void ensureFolder() {
        system("if not exist data mkdir data");
    }

public:
    void saveUser(int UserId, const string& Name, int Streak, float TotalMins) {
        ensureFolder();
        vector<string> Lines;
        ifstream Fin(UsersFile);
        string Line;
        bool Found=false;
        while(getline(Fin, Line)) {
            stringstream Ss(Line);
            string Tok;
            getline(Ss, Tok, ',');
            if(stoi(Tok)==UserId) {
                Lines.push_back(to_string(UserId)+","+Name+","+to_string(Streak)+","+to_string(TotalMins));
                Found=true;
            } else {
                Lines.push_back(Line);                                  // If the line does not match the user ID, keep it unchanged
            }
        }
        Fin.close();
        if(!Found)
            Lines.push_back(to_string(UserId)+","+Name+","+to_string(Streak)+","+to_string(TotalMins));
        ofstream Fout(UsersFile);
        for(auto& L : Lines)
            Fout<<L<<"\n";
        Fout.close();
    }

    void saveSubjects(int UserId, const vector<Subject>& Subjects) {
        ensureFolder();
        vector<string> Lines;
        ifstream Fin(SubjectsFile);
        string Line;
        while(getline(Fin, Line))
            if(Line.rfind(to_string(UserId)+"|", 0)!=0)
                Lines.push_back(Line);
        Fin.close();
        ofstream Fout(SubjectsFile);
        for(auto& L : Lines)
            Fout<<L<<"\n";
        for(auto& S : Subjects)
            Fout<<UserId<<"|"<<S.WriteFile()<<"\n";
        Fout.close();
    }

    void saveTasks(int UserId, const vector<Subject>& Subjects) {
        ensureFolder();
        vector<string> Lines;
        ifstream Fin(TasksFile);
        string Line;
        while(getline(Fin, Line))
            if(Line.rfind(to_string(UserId)+"|", 0)!=0)
                Lines.push_back(Line);
        Fin.close();
        ofstream Fout(TasksFile);
        for(auto& L : Lines)
            Fout<<L<<"\n";
        for(auto& S : Subjects)
            for(auto& T : S.getTasks())
                Fout<<UserId<<"|"<<S.getName()<<"|"<<T.WriteFile()<<"\n";
        Fout.close();
    }

    void saveSessions(const vector<StudySession>& Sessions) {
        ensureFolder();
        ofstream Fout(SessionsFile);
        for(auto& S : Sessions)
            Fout<<S.WriteFile()<<"\n";
        Fout.close();
    }

    bool loadUser(int UserId, string& Name, int& Streak, float& TotalMins) {
        ifstream Fin(UsersFile);
        if(!Fin)
            return false;
        string Line;
        while(getline(Fin, Line)) {
            stringstream Ss(Line);
            string Tok;
            getline(Ss, Tok, ',');
            int Id=stoi(Tok);
            if(Id==UserId) {
                getline(Ss, Tok, ','); Name=Tok;
                getline(Ss, Tok, ','); Streak=stoi(Tok);
                getline(Ss, Tok, ','); TotalMins=stof(Tok);
                Fin.close();
                return true;
            }
        }
        Fin.close();
        return false;
    }

    void loadSubjectsAndTasks(int UserId, vector<Subject>& Subjects) {
        Subjects.clear();
        ifstream Fin(SubjectsFile);
        string Line;
        while(getline(Fin, Line))
            if(Line.rfind(to_string(UserId)+"|", 0)==0)
                Subjects.push_back(Subject::ReadFile(Line.substr(Line.find('|')+1)));
        Fin.close();
        ifstream Ft(TasksFile);
        while(getline(Ft, Line)) {
            if(Line.rfind(to_string(UserId)+"|", 0)==0) {
                string Rest=Line.substr(Line.find('|')+1);
                string SubName=Rest.substr(0, Rest.find('|'));
                string TaskData=Rest.substr(Rest.find('|')+1);
                Task T=Task::ReadFile(TaskData);
                for(auto& S : Subjects)
                    if(S.getName()==SubName)
                        S.getTasks().push_back(T);
            }
        }
        Ft.close();
    }

    void loadSessions(vector<StudySession>& Sessions) {
        Sessions.clear();
        ifstream Fin(SessionsFile);
        if(!Fin)
            return;
        string Line;
        while(getline(Fin, Line))
            if(!Line.empty())
                Sessions.push_back(StudySession::ReadFile(Line));
        Fin.close();
    }

    void loadAll(int UserId, string& Name, int& Streak, float& TotalMins, vector<Subject>& Subjects, vector<StudySession>& Sessions) {
        loadUser(UserId, Name, Streak, TotalMins);
        loadSubjectsAndTasks(UserId, Subjects);
        loadSessions(Sessions);
    }
};

class Person {  // Base class representing a person with a user ID and name, and a virtual method to display their profile      
protected:
    int UserId;
    string Name;

public:
    Person(int Id, string N) : UserId(Id), Name(N) {}
    virtual ~Person() {}
    virtual void displayProfile()=0;
    int getId() const { 
        return UserId; }
    string getName() const { 
        return Name; }
};

class User : public Person {
private:
    int Streak, BestStreak, TaskCount;
    float TotalMins;
    vector<Subject> Subjects;
    vector<StudySession> Sessions;
    Scheduler Sched;
    Analytics Stats;
    FileManager FileMgr;

public:
    User(int Id, string N)
        : Person(Id, N), Streak(0), TotalMins(0), BestStreak(0), TaskCount(1) {}

    void displayProfile() override {
        cpk_line('=');  // Print header for today's plan
        cout<<DIM_PURPLE<<"| "<<RESET<<NEON_PURPLE<<BOLD<<left<<setw(43)<<"  USER PROFILE"<<RESET<<DIM_PURPLE<<"|"<<RESET<<"\n";
        cpk_line('=');  // Print header for today's plan
        cout<<DIM_PURPLE<<"| "<<RESET<<DIM_WHITE<<"  Name         : "<<NEON_GREEN<<left<<setw(26)<<Name<<RESET<<DIM_PURPLE<<"|"<<RESET<<"\n";
        cout<<DIM_PURPLE<<"| "<<RESET<<DIM_WHITE<<"  User ID      : "<<NEON_CYAN<<left<<setw(26)<<UserId<<RESET<<DIM_PURPLE<<"|"<<RESET<<"\n";
        cout<<DIM_PURPLE<<"| "<<RESET<<DIM_WHITE<<"  Streak       : "<<NEON_ORANGE<<left<<setw(26)<<(to_string(Streak)+" day(s)")<<RESET<<DIM_PURPLE<<"|"<<RESET<<"\n";
        cout<<DIM_PURPLE<<"| "<<RESET<<DIM_WHITE<<"  Total Study  : "<<NEON_YELLOW<<left<<setw(26)<<(to_string((int)(TotalMins/60))+"h "+to_string((int)fmod(TotalMins,60))+"m")<<RESET<<DIM_PURPLE<<"|"<<RESET<<"\n";
        cout<<DIM_PURPLE<<"| "<<RESET<<DIM_WHITE<<"  Subjects     : "<<NEON_PURPLE<<left<<setw(26)<<Subjects.size()<<RESET<<DIM_PURPLE<<"|"<<RESET<<"\n";
        cpk_line('=');  // Print header for today's plan
    }

    void addSubject() {  // Add a new subject by taking user input for subject name, difficulty, and deadline, and then save it to filevb   
        string SubName, DeadlineStr;
        int Diff;
        cpk_section("Add New Subject");
        cout<<DIM_WHITE<<"  Subject name     : "<<NEON_CYAN;
        cin.ignore();  // Take user input for task topic, estimated duration, and deadline, and then save the new task to file
        getline(cin, SubName);
        cout<<RESET;
        cout<<DIM_WHITE<<"  Difficulty (1-5) : "<<NEON_YELLOW;
        while(!(cin>>Diff)||Diff<1||Diff>5) {
            cout<<NEON_PINK<<"  Enter 1-5: "<<RESET;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cout<<RESET;
        cout<<DIM_WHITE<<"  Deadline (YYYY-MM-DD): "<<NEON_PURPLE;
        cin>>DeadlineStr;
        cout<<RESET;
        Subjects.push_back(Subject(SubName, Diff, DeadlineStr));
        FileMgr.saveSubjects(UserId, Subjects);
        cout<<NEON_GREEN<<"\n  Subject \""<<SubName<<"\" added!\n"<<RESET;
        cpk_line();  // Print message if there are no urgent tasks
    }

    void addTask() {  // Add a new task to an existing subject
        if(Subjects.empty()) {
            cout<<NEON_YELLOW<<"  Please add a subject first.\n"<<RESET;
            return;
        }
        cpk_section("Add Task");
        cout<<DIM_WHITE<<"  Choose subject:\n"<<RESET;
        for(int I=0; I<(int)Subjects.size(); I++)
            cout<<"    "<<NEON_PINK<<(I+1)<<". "<<NEON_CYAN<<Subjects[I].getName()<<RESET<<"\n";
        int Choice;
        cout<<NEON_PURPLE<<"  > "<<RESET;
        while(!(cin>>Choice)||Choice<1||Choice>(int)Subjects.size()) {
            cout<<NEON_PINK<<"  Invalid choice: "<<RESET;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        Subject& S=Subjects[Choice-1];
        string Topic, DeadlineStr;
        float Mins;
        cin.ignore();  // Take user input for task topic, estimated duration, and deadline, and then save the new task to file
        cout<<DIM_WHITE<<"  Task topic            : "<<NEON_CYAN;
        getline(cin, Topic);
        cout<<RESET;
        cout<<DIM_WHITE<<"  Est. duration (min)   : "<<NEON_YELLOW;
        cin>>Mins;
        cout<<RESET;
        cout<<DIM_WHITE<<"  Deadline (YYYY-MM-DD) : "<<NEON_PURPLE;
        cin>>DeadlineStr;
        cout<<RESET;
        string Num=to_string(TaskCount);
        while((int)Num.size()<3)
            Num="0"+Num;
        string TId="T"+to_string(UserId)+Num;
        TaskCount++;
        Task T(TId, Topic, Mins, Date::fromString(DeadlineStr));
        S.addTask(T);
        FileMgr.saveTasks(UserId, Subjects);
        cpk_line();  // Print message if there are no urgent tasks
    }

    void viewSchedule() {  //display the schedule
        Sched.loadTasks(Subjects);
        int Choice;
        cpk_section("Schedule View");
        cout<<"  "<<NEON_PINK<<"1. "<<DIM_WHITE<<"Today's Plan (top 5 priority)\n";
        cout<<"  "<<NEON_PINK<<"2. "<<DIM_WHITE<<"Full Generated Schedule\n";
        cout<<"  "<<NEON_PINK<<"3. "<<DIM_WHITE<<"Weekly Overview\n";
        cout<<NEON_PURPLE<<"  > "<<RESET;
        cin>>Choice;
        if(Choice==1)
            Sched.printDailyPlan();
        else if(Choice==2)
            Sched.generateSchedule();
        else if(Choice==3)
            Sched.printWeeklyPlan();
        else
            cout<<NEON_PINK<<"  Invalid choice.\n"<<RESET;
    }

    void viewAnalytics() {  // Display various analytics based on the user's subjects and study sessions
        int Choice;
        cpk_section("Analytics Dashboard");
        cout<<"  "<<NEON_PINK<<"1. "<<DIM_WHITE<<"Subject-wise progress\n";
        cout<<"  "<<NEON_PINK<<"2. "<<DIM_WHITE<<"Weak subject detector\n";
        cout<<"  "<<NEON_PINK<<"3. "<<DIM_WHITE<<"Study hours chart\n";
        cout<<"  "<<NEON_PINK<<"4. "<<DIM_WHITE<<"Streak tracker\n";
        cout<<"  "<<NEON_PINK<<"5. "<<DIM_WHITE<<"Session comparison (planned vs actual)\n";
        cout<<NEON_PURPLE<<"  > "<<RESET;
        cin>>Choice;
        if(Choice==1)
            Stats.subjectWiseProgress(Subjects);
        else if(Choice==2)
            Stats.weakSubjectDetector(Subjects);
        else if(Choice==3)
            Stats.studyHoursChart(Sessions);
        else if(Choice==4)
            Stats.streakTracker(Streak, BestStreak);
        else if(Choice==5)
            Stats.sessionComparison(Sessions);
        else
            cout<<NEON_PINK<<"  Invalid choice.\n"<<RESET;
    }

    void markTask() {  //mark task as completed 
        cpk_section("Mark Task as Completed");
        string TId;
        cout<<DIM_WHITE<<"  Enter Task ID: "<<NEON_CYAN;
        cin>>TId;
        cout<<RESET;
        for(auto& S : Subjects) {
            vector<Task>& Tasks=S.getTasks();
            for(int I=0; I<(int)Tasks.size(); I++) {
                if(Tasks[I].getTaskId()==TId) {
                    Tasks[I].markCompleted();
                    float Actual;
                    cout<<DIM_WHITE<<"  Minutes actually studied: "<<NEON_YELLOW;
                    cin>>Actual;
                    cout<<RESET;
                    Tasks[I].setActualMins(Actual);
                    TotalMins+=Actual;
                    Date Today=Date::GetTodayDate();
                    StudySession Sess(TId, Today.StringConvert(), (int)Tasks[I].getEstMins(), (int)Actual);
                    Sess.logSession();
                    Sessions.push_back(Sess);
                    Streak++;
                    if(Streak>BestStreak)
                        BestStreak=Streak;
                    FileMgr.saveTasks(UserId, Subjects);
                    FileMgr.saveSessions(Sessions);
                    FileMgr.saveUser(UserId, Name, Streak, TotalMins);
                    return;
                }
            }
        }
        cout<<NEON_PINK<<"  Task ID not found.\n"<<RESET;
        cpk_line();  // Print message if there are no urgent tasks
    }

    void reschedule() {  //reschedule the task and add penalty if missed
        Sched.rescheduleMissed(Subjects);
        FileMgr.saveTasks(UserId, Subjects);
        FileMgr.saveSubjects(UserId, Subjects);
    }

    void checkUrgent() {  // Check for urgent tasks and display alerts
        Sched.loadTasks(Subjects);
        Sched.flagUrgentTasks();
    }

    void listSubjects() {  // List all subjects with their details and progress
        if(Subjects.empty()) {
            cout<<NEON_YELLOW<<"  No subjects added yet.\n"<<RESET;
            return;
        }
        cpk_section("Your Subjects");
        for(int I=0; I<(int)Subjects.size(); I++) {
            float Pct=Subjects[I].completionPercent();
            cout<<"  "<<NEON_PINK<<(I+1)<<". "<<NEON_CYAN<<left<<setw(18)<<Subjects[I].getName()<<DIM_WHITE<<"Diff: "<<NEON_YELLOW<<Subjects[I].getDiff()<<DIM_WHITE<<"  Deadline: "<<NEON_PURPLE<<Subjects[I].getDeadline()<<"  "<<neonBar(Pct,10)<<priorityTag(Pct)<<RESET<<"\n";
        }
        cpk_line();  // Print message if there are no urgent tasks
    }

    void viewTasksOfSubject() {  // View tasks of a specific subject by listing subjects
        listSubjects();
        if(Subjects.empty())
            return;
        int Choice;
        cout<<NEON_PURPLE<<"  Choose subject number: "<<RESET;
        cin>>Choice;
        if(Choice<1||Choice>(int)Subjects.size()) {
            cout<<NEON_PINK<<"  Invalid.\n"<<RESET;
            return;
        }
        Subjects[Choice-1].listTasks();
    }

    void loadData() {  // Load all user data
        string LoadedName;
        int LoadedStreak=0;
        float LoadedMins=0;
        FileMgr.loadAll(UserId, LoadedName, LoadedStreak, LoadedMins, Subjects, Sessions);
        if(LoadedStreak>0)
            Streak=LoadedStreak;
        if(LoadedMins>0)
            TotalMins=LoadedMins;
    }
};

int getInt(const string& Prompt) {
    int Val;
    cout<<DIM_WHITE<<Prompt<<NEON_CYAN;
    while(!(cin>>Val)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout<<NEON_PINK<<"  Please enter a valid number: "<<RESET;
    }
    cout<<RESET;
    return Val;
}

void printBanner() {  // Print the welcome banner with the current date   
    Date Today=Date::GetTodayDate();
    cout<<"\n"<<NEON_PURPLE;
    cout<<"_______________________________________________"<<"\n";
    cout<<"  "<<BOLD<<"SMART STUDY PLANNER"<<RESET<<NEON_PURPLE<<"                      "<<"\n";
    cout<<"  "<<DIM_WHITE<<"CS112 OOP | C++ Console"<<NEON_PURPLE<<"               "<<"\n";
    cout<<"_______________________________________________"<<"\n";
    cout<<"  "<<DIM_PURPLE<<"Date : "<<NEON_CYAN<<Today.StringConvert()<<NEON_PURPLE<<"                    "<<"\n";
    cout<<"_______________________________________________"<<RESET<<"\n";
}

void userMenu(User& UserRef) {
    int Choice=-1;
    while(Choice!=0) {
        cout<<"\n"<<NEON_PURPLE<<"_______________________________________________"<<RESET<<"\n";
        cout<<NEON_PURPLE<<RESET<<NEON_YELLOW<<BOLD<<"*************DASHBOARD MENU*******************"<<RESET<<NEON_PURPLE<<RESET<<"\n";
        cout<<NEON_PURPLE<<"_______________________________________________"<<RESET<<"\n";
        const char* Items[]={
            "1.  View Profile",
            "2.  Add Subject",
            "3.  Add Task to Subject",
            "4.  View My Subjects",
            "5.  View Tasks of a Subject",
            "6.  View / Generate Schedule",
            "7.  Mark Task as Completed",
            "8.  Reschedule Missed Tasks",
            "9.  Analytics Dashboard",
            "10. Check Urgent Alerts",
            "0.  Logout"
        };
        for(int I=0; I<11; I++) {
            cout<<NEON_PURPLE<<"| "<<RESET;
            cout<<(I<9?NEON_PINK:NEON_ORANGE)<<Items[I];
            int Len=(int)strlen(Items[I]);
            for(int J=Len; J<43; J++)
                cout<<" ";
            cout<<NEON_PURPLE<<"|"<<RESET<<"\n";
        }
        cout<<NEON_PURPLE<<"_______________________________________________"<<RESET<<"\n";
        cout<<NEON_GREEN<<"> "<<RESET;
        cin>>Choice;
        switch(Choice) {
            case 1: 
                UserRef.displayProfile(); 
                break;
            case 2: 
                UserRef.addSubject(); 
                break;
            case 3: 
                UserRef.addTask(); 
                break;
            case 4: 
                UserRef.listSubjects(); 
                break;
            case 5: 
                UserRef.viewTasksOfSubject(); 
                break;
            case 6: 
                UserRef.viewSchedule(); 
                break;
            case 7: 
                UserRef.markTask(); 
                break;
            case 8: 
                UserRef.reschedule();  
                break;
            case 9: 
                UserRef.viewAnalytics(); 
                break;
            case 10: 
                UserRef.checkUrgent(); 
                break;
            case 0:
                cout<<NEON_CYAN<<"\n  Logging out... Stay sharp.\n"<<RESET;
                cpk_line('=');  // Print header for today's plan
                return;
            default:
                cout<<NEON_PINK<<"  Invalid option.\n"<<RESET;
        }
    }
}