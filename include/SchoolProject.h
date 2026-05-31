#pragma once

#include "Project.h"

// A project tied to a particular school class/course. Adds the class field
// on top of everything a Project carries.
class SchoolProject : public Project
{
public:
    SchoolProject(const string& name, const string& description,
                  const Date& deadline, const Priority& priority,
                  const string& schoolClass);
    // Reads a school project body (the type tag has already been consumed
    // by the loader); the base data is read first, then the school class.
    explicit SchoolProject(istream& in);
    ~SchoolProject() override;

    const string& getSchoolClass() const;

    // Appends the school class to the base project's header line.
    void printHeader(ostream& out) const override;

    // Writes the school type tag, the base data, then the school class.
    void write(ostream& out) const override;

private:
    const string schoolClass;
};
