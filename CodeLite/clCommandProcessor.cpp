#include "clCommandProcessor.h"
#include "cl_command_event.h"
#include "processreaderthread.h"

wxDEFINE_EVENT(wxEVT_COMMAND_PROCESSOR_ENDED, clCommandEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_PROCESSOR_OUTPUT, clCommandEvent);

BEGIN_EVENT_TABLE(clCommandProcessor, wxEvtHandler)
EVT_COMMAND(wxID_ANY, wxEVT_PROC_DATA_READ, clCommandProcessor::OnProcessOutput)
EVT_COMMAND(wxID_ANY, wxEVT_PROC_TERMINATED, clCommandProcessor::OnProcessTerminated)
END_EVENT_TABLE()

clCommandProcessor::clCommandProcessor(const wxString& command,
                                       const wxString& wd,
                                       size_t processFlags)
    : m_next(NULL)
    , m_prev(NULL)
    , m_process(NULL)
    , m_command(command)
    , m_workingDirectory(wd)
    , m_processFlags(processFlags)
{
}

clCommandProcessor::~clCommandProcessor() { wxDELETE(m_process); }

void clCommandProcessor::ExecuteCommand()
{
    wxString message;
    message << _("Executing: ") << m_command << " [ wd: " << m_workingDirectory << " ]";
    
    clCommandEvent eventStart(wxEVT_COMMAND_PROCESSOR_OUTPUT);
    eventStart.SetString(message);
    GetFirst()->ProcessEvent(eventStart);

    m_process = ::CreateAsyncProcess(this, m_command, m_processFlags, m_workingDirectory);
    if(!m_process) {
        clCommandEvent eventEnd(wxEVT_COMMAND_PROCESSOR_ENDED);
        eventEnd.SetString(wxString::Format(_("Failed to execute command: %s"), m_command));
        GetFirst()->ProcessEvent(eventEnd);
        DeleteChain();
    }
}

void clCommandProcessor::OnProcessOutput(wxCommandEvent& event)
{
    ProcessEventData* ped = (ProcessEventData*)event.GetClientData();
    clCommandEvent eventStart(wxEVT_COMMAND_PROCESSOR_OUTPUT);
    eventStart.SetString(ped->GetData());
    GetFirst()->ProcessEvent(eventStart);
    wxDELETE(ped);
}

void clCommandProcessor::OnProcessTerminated(wxCommandEvent& event)
{
    ProcessEventData* ped = (ProcessEventData*)event.GetClientData();
    wxDELETE(ped);

    if(m_next) {
        // more commands, don't report an 'END' event
        m_next->ExecuteCommand();
        
    } else {
        // no more commands to execute, delete the entire chain
        clCommandEvent eventEnd(wxEVT_COMMAND_PROCESSOR_ENDED);
        GetFirst()->ProcessEvent(eventEnd);
        DeleteChain();
    }
}

clCommandProcessor* clCommandProcessor::Link(clCommandProcessor* next)
{
    this->m_next = next;
    if(m_next) {
        m_next->m_prev = this;
    }
    return next;
}

void clCommandProcessor::DeleteChain()
{
    // Move to the first one in the list
    clCommandProcessor* first = GetFirst();

    // delete
    while(first) {
        clCommandProcessor* next = first->m_next;
        wxDELETE(first);
        first = next;
    }
}

clCommandProcessor* clCommandProcessor::GetFirst()
{
    clCommandProcessor* first = this;
    while(first->m_prev) {
        first = first->m_prev;
    }
    return first;
}