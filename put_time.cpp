#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>

int main() {
    using namespace boost::posix_time;
    auto t = microsec_clock::universal_time();
    std::cout << to_iso_extended_string(t) << "Z\n";
}

#if 0
int main()
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::cout.imbue(std::locale("ru_RU.utf8"));
    std::cout << "ru_RU: " << std::put_time(now, "%c %z") << '\n';
    std::cout.imbue(std::locale("ja_JP.utf8"));
    std::cout << "ja_JP: " << std::put_time(&tm, "%c %Z") << '\n';
}
#endif
