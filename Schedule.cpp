#include<iostream>
#include<iomanip>
#include<fstream>
#include<string>
#include<vector>
#include<set>
#include<map>
#include<algorithm>
#include<queue>
#include<cmath>
#include<cassert>
using namespace std; 


class Class{
    public:
    char name;
    int grade;
    
    Class(char name_, int grade_)
    {
        name=name_;
        grade=grade_;

    }

    Class():name('A'),grade(9){
        
    }

    void display()
    {
        std::cout << "Name: " << name << "\tgrade: " << grade << std::endl;
    }
    
};

enum Days{
    Mon=0,
    Tue,
    Wed,
    Thu,
    Fri,
    Sat
};

enum LessonType{
    fizmat=0,
    gum
};

struct Lesson{
    string name;
    LessonType type;
    int count;
    int teacher_id;
    bool is_dual;
    int complexity;
};

struct Day{
    vector<Lesson> lessons;
    Days name;
    int count_dual(){
        set<string> st;//уникальные предметы
        for(auto l:lessons) st.insert(l.name);
        return lessons.size()-st.size();
    }

    Day get_shifted_day(){
        Day d;
        d.name=name;
        int shift=1;
        if(count_dual()>0)
            shift=2;
        for(int i=0;i<lessons.size(); i++)
            d.lessons.push_back(lessons[(i+shift)%lessons.size()]);
        return d;
    }

    int get_complexity(){
        int sam=0;
        //todo: спросить у составителя суммарная или средняя
        for(int i=0;i<lessons.size();i++)
            sam+=lessons[i].complexity; 
        return sam;
    }
    
    int lesson_count(Lesson l){
        int cnt=0;
        for(auto i: lessons)
            if(i.name==l.name)
                cnt++;
        return cnt;
    }
};

class Schedule{
public:
    vector<Lesson> lessons;
    vector<pair<Class,vector<Day>>> parallel;
    int grade;
    static const int max_lessons_day=6;
    static const int max_equal_lessons=2;
    static const int max_count_dual=2;
    static const int days_at_week=6;
    int graph_of_complexity[days_at_week]={1, 3, 4, 2, 5, 0};

    Schedule(int grade, int count_of_classes) :grade(grade)
    {
        vector<Day> week;
        Day d;
        d.name=Days::Mon;
        for(int i=0;i<days_at_week;i++)
        {
            week.push_back(d);
            d.name=Days(int(d.name)+1);
        }
        Class c;
        c.name='A';
        c.grade=grade;
        for(int i=0;i<count_of_classes;i++){
            parallel.push_back(make_pair(c,week));
            c.name++;
        }
    }

    Schedule():Schedule( 9, 4){ }

private:
    
public:
    void generate(string lessons_path) 
    {
        set_lessons(lessons_path);
        gen_first();
        //gen_other();
    }
private:
    void gen_other(){
         //TODO: Дописать
        for(int i=1;i<parallel.size();i++)
            for(int j=0;j<parallel[i].second.size();j++)
                parallel[i].second[j]=parallel[i-1].second[j].get_shifted_day();
    }

    void gen_first(){
        queue<Lesson> lq;
        for(auto l:lessons) lq.push(l);
        int day=0;
        int step=0;
     
        fill(max_lessons_day, lq);
        fill(max_lessons_day+1,lq, false);
        
        parallel[0].second=sort_by_complexity(parallel[0].second);
    }

    void fill(int _max_lessons_day, queue<Lesson>& lq, bool first=true){
        int day=0;
        int step=0;
        auto already_fill=[*this,max_lessons_day](){
            for(auto day:parallel[0].second)
                if(day.lessons.size()<max_lessons_day)
                    return false;
            return true;
        };
        cout<<lq.size()<<endl;
        while((!lq.empty() && first && !already_fill()) ||
                (!lq.empty() && !first)){
            step++;
            if(step>1000)
                assert(false);//"Ne udalos sgenerit raspisanie dlya A classa")
            
            if(parallel[0].second[day].lessons.size()>=_max_lessons_day)//если уроков уже полно в этот день
            {
                day=(day+1)%parallel[0].second.size();//переключаем день
                continue;
            }

            while(lq.front().count>0)
            {
                //todo: next raz условие настроить
                if(lq.front().is_dual)
                    if(!(parallel[0].second[day].lesson_count(lq.front())<max_equal_lessons 
                        && parallel[0].second[day].count_dual()<max_count_dual))
                        break;

                parallel[0].second[day].lessons.push_back(lq.front());
                lq.front().count--;
                if(!lq.front().is_dual) break;
            }
            
            auto t=lq.front();
            lq.pop();//удаляем урок из очереди
            if(t.count!=0) lq.push(t); //добавляем остаток в конец очереди
        }
    }
    
    vector<Day> sort_by_complexity(vector<Day> days){
        sort(days.begin(),
            days.end(),
            [](Day l, Day r){return l.get_complexity()<r.get_complexity();});//лямбда c++
        vector<Day> ans;
        for(int i=0;i<days_at_week;i++)
        {
            days[ graph_of_complexity[i] ].name=Days(int(Days::Mon)+i);
            ans.push_back(days[ graph_of_complexity[i] ]);
        }
        return ans;
    }


    void set_lessons(string lessons_path){
        ifstream in(lessons_path);
        if(!in.good()){
            assert(false);//"file error"
        }
        while(1){
            Lesson l;
            in>>l.name>>l.count>>l.teacher_id>>l.is_dual>> l.complexity;
            if(in.eof()) break;

            lessons.push_back(l);
        }
        in.close();
        
        for(auto l:lessons)
        {
            cout<<l.name<<" "<<l.teacher_id<<" "<<l.count<<" "<<l.is_dual<<" "<<l.complexity<<endl;
        }
    }

public:
    void print(){
        for(auto c : parallel){
            cout << c.first.grade << c.first.name << endl;
            for(Day d:c.second)
            {
                cout<<setw(15)<<d.name<<" "<<d.get_complexity()<<":\t\t";
                for(auto l:d.lessons)//l один из уроков в этом дне
                {
                    cout << l.name<< " \t";
                }
                cout<<endl;
            }
            cout<<endl;
        }
    }

    void write_csv(string filename){
        ofstream out(filename);
        if(!out.good())
            assert(false);//"file error"

        for(auto c : parallel){
            out << c.first.grade << c.first.name << endl;
            for(Day d:c.second)
            {
                out<<setw(15)<<d.name<<";";
                for(auto l:d.lessons)//l один из уроков в этом дне
                {
                    out << l.name<< ";";
                }
                out<<endl;
            }
            out<<endl;
        }
        out.close();
    }

};





int main()
{
    Schedule shd(9,4);
    shd.generate("lessons.txt");
    shd.print();
    shd.write_csv("rasp.csv");
    return 0;
}