#include "Task.h"

Task::Task()
{
}

Task::Task(const string& title, const string& description, const Date& deadline,
           Priority priority, Status status)
    : title(title), description(description), deadline(deadline),
      priority(priority), status(status)
{
}

Task::~Task()
{
}

bool Task::start()
{
    if (status != Status::NotStarted)
    {
        return false;
    }

    status = Status::InProgress;
    return true;
}

bool Task::finish()
{
    if (status != Status::InProgress)
    {
        return false;
    }

    status = Status::Completed;
    return true;
}

const string& Task::getTitle() const
{
    return title;
}

const string& Task::getDescription() const
{
    return description;
}

const Date& Task::getDeadline() const
{
    return deadline;
}

Priority Task::getPriority() const
{
    return priority;
}

Status Task::getStatus() const
{
    return status;
}
