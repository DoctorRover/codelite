#ifndef PHPENTITYCLASSIMPL_H
#define PHPENTITYCLASSIMPL_H

#include "codelite_exports.h"
#include "PHPEntityBase.h" // Base class: PHPEntityBase

class WXDLLIMPEXP_CL PHPEntityClass : public PHPEntityBase
{
    wxString m_extends;
    wxArrayString m_implements;
    wxArrayString m_traits;

public:
    virtual wxString Type() const;
    // Save the class into teh database
    virtual void Store(wxSQLite3Database& db);
    virtual void FromResultSet(wxSQLite3ResultSet& res);

    virtual wxString ID() const;
    virtual void PrintStdout(int indent) const;

    /**
     * @brief return an array of inheritance (extends, implementes and traits)
     */
    wxArrayString GetInheritanceArray() const;

    PHPEntityClass();
    virtual ~PHPEntityClass();
    void SetExtends(const wxString& extends) { this->m_extends = extends; }
    const wxString& GetExtends() const { return m_extends; }
    void SetImplements(const wxArrayString& implements) { this->m_implements = implements; }
    const wxArrayString& GetImplements() const { return m_implements; }
    void SetTraits(const wxArrayString& traits) { this->m_traits = traits; }
    const wxArrayString& GetTraits() const { return m_traits; }
    wxString GetImplementsAsString() const { return ::wxJoin(m_implements, ';'); }
    wxString GetTraitsAsString() const { return ::wxJoin(m_traits, ';'); }
};

#endif // PHPENTITYCLASSIMPL_H