#ifndef __acceltabledlg__
#define __acceltabledlg__

/**
@file
Subclass of AccelTableBaseDlg, which is generated by wxFormBuilder.
*/

#include "acceltablebasedlg.h"

/** Implementing AccelTableBaseDlg */
class AccelTableDlg : public AccelTableBaseDlg
{
	long m_selectedItem;
	
protected:
	// Handlers for AccelTableBaseDlg events.
	void OnItemActivated( wxListEvent& event );
	void OnItemSelected( wxListEvent& event );
	void OnItemDeselected( wxListEvent& event );
	void OnColClicked(wxListEvent &event);
	void PopulateTable();
	
public:
	/** Constructor */
	AccelTableDlg( wxWindow* parent );
};

#endif // __acceltabledlg__
