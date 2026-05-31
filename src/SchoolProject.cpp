#include "SchoolProject.h"

SchoolProject::SchoolProject(const string& name, const string& description,
                             const Date& deadline, const Priority& priority,
                             const string& schoolClass)
    : Project(name, description, deadline, priority), schoolClass(schoolClass)
{
}

SchoolProject::SchoolProject(istream& in)
    : Project(in), schoolClass(readLine(in))
{
}

SchoolProject::~SchoolProject()
{
}

const string& SchoolProject::getSchoolClass() const
{
    return schoolClass;
}

void SchoolProject::printHeader(ostream& out) const
{
    Project::printHeader(out);
    out << " | class: " << schoolClass;
}

void SchoolProject::write(ostream& out) const
{
    out << static_cast<int>(ProjectType::School) << '\n';
    writeBody(out);
    out << schoolClass << '\n';
}
