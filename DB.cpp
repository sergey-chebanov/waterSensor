#include <iostream>
#include <sqlite_modern_cpp.h>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "DB.h"

using namespace std;

sqlite::database _db("waterDB.sqlite");

bool putMeasurement(int hot, int cold) {
    using namespace boost::posix_time;
    using namespace sqlite;

    try {

        string const datetime =  to_iso_extended_string(microsec_clock::universal_time()) + "Z\n";

        cout << datetime << endl;

        _db << "insert into water (datetime,hot,cold) values (?,?,?);"
            << datetime
            << hot
            << cold;
    }
    catch (exception& e) {
        cerr << e.what();
        return false;
    }
    return true;
}

#if 0

int main() {

    try {

        putMeasurement(1,2);
        putMeasurement(3,4);
        putMeasurement(5,6);



        // creates a database file 'dbfile.db' if it does not exists.
        // inserts a new user record.
        // binds the fields to '?' .
        // note that only types allowed for bindings are :
        //      int ,long, long long, float, double
        //      string , u16string
        // sqlite3 only supports utf8 and utf16 strings, you should use std::string for utf8 and std::u16string for utf16.
        // note that u"my text" is a utf16 string literal of type char16_t * .
        db << "insert into user (age,name,weight) values (?,?,?);"
            << 20
            << u"bob"
            << 83.25;

        int age = 21;
        float weight = 68.5;
        string name = "jack";
        db << u"insert into user (age,name,weight) values (?,?,?);" // utf16 query string
            << age
            << name
            << weight;

        cout << "The new record got assigned id " << db.last_insert_rowid() << endl;

        // slects from user table on a condition ( age > 18 ) and executes
        // the lambda for each row returned .
        db << "select age,name,weight from user where age > ? ;"
            << 18
            >> [&](int age, string name, double weight) {
            cout << age << ' ' << name << ' ' << weight << endl;
        };

        // selects the count(*) from user table
        // note that you can extract a single culumn single row result only to : int,long,long,float,double,string,u16string
        int count = 0;
        db << "select count(*) from user" >> count;
        cout << "cout : " << count << endl;

        // you can also extract multiple column rows
        db << "select age, name from user where _id=1;" >> tie(age, name);
        cout << "Age = " << age << ", name = " << name << endl;

        // this also works and the returned value will be automatically converted to string
        string str_count;
        db << "select count(*) from user" >> str_count;
        cout << "scount : " << str_count << endl;

    }
    catch (exception& e) {
        cout << e.what() << endl;
    }
}

#endif