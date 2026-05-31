#include "Task.h"

Task::Task()
    : priority(Priority::Low), status(Status::NotStarted)
{
}

Task::Task(const string& title, const string& description,
           const Priority& priority, Status status)
    : title(title), description(description),
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

bool Task::cancel()
{
    if (status != Status::NotStarted && status != Status::InProgress)
    {
        return false;
    }

    status = Status::Canceled;
    return true;
}

bool Task::omit()
{
    if (status != Status::NotStarted && status != Status::InProgress)
    {
        return false;
    }

    status = Status::Omitted;
    return true;
}

bool Task::increasePriority()
{
    if (priority == Priority::High)
    {
        return false;
    }

    priority = static_cast<Priority>(static_cast<int>(priority) + 1);
    return true;
}

bool Task::decreasePriority()
{
    if (priority == Priority::Low)
    {
        return false;
    }

    priority = static_cast<Priority>(static_cast<int>(priority) - 1);
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

Priority Task::getPriority() const
{
    return priority;
}

Status Task::getStatus() const
{
    return status;
}
