/*
 *  Copyright (C) 2015, 2016 University of Leeds
    Copyright (C) 2016, 2017 UCL
    This file is part of STIR.

    This file is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This file is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    See STIR/LICENSE.txt for details
*/
/*!
  \file
  \ingroup listmode
  \brief Declaration of class stir::CListModeDataROOT

  \author Nikos Efthimiou
  \author Harry Tsoumpas
  \author Kris Thielemans
*/

#ifndef __stir_listmode_CListModeDataROOT_H__
#define __stir_listmode_CListModeDataROOT_H__

#include "stir/listmode/CListModeData.h"
#include "stir/listmode/CListRecordROOT.h"
#include "stir/IO/InputStreamFromROOTFile.h"
#include "stir/KeyParser.h"

START_NAMESPACE_STIR

/*!
  \brief class that reads ROOT files, generated by GATE, as listmode data.
  \author Nikos Efthimiou
  \author Kris Thielemans

  This class does not directly read the ROOT file but instead needs an Interfile-type
  header to specify the scanner (as this information is not stored in the ROOT file).
  We tend to name this header <tt>something.hroot</tt> but this is not mandatory.

  \warning There is currently no check if the scanner information is correct. This
  is dangerous for the geometry, but can also lead to crashes if the actual number of 
  blocks/crystals etc is larger than what is specified in the scanner info.

  We currently support only ROOT output using the 'Cylindrical PET' and 'ECAT' systems
  from GATE.

  The Scanner geometry is set in multiple places. In hroot there is the option to use the
  Scanner name to pull a Scanner from the library. Another option is to use
  Scanner::User_defined_scanner and set the members: num_rings, num_detectors_per_ring,
  max_num_non_arccorrected_bins, inner_ring_diameter, average_depth_of_interaction,
  ring_spacing and bin_size, manually.

  In any case this scanner must checkout the geometry created by the repeaters section
  (which will initialise the InputStreamFromROOTFile.
  And of course, match the Scanner used by other parts of STIR (e.g. ProjDataInfo e.t.c).

  Finally, in the case that Scanner::Unknown_scanner the algorithm will
  try to create the scanner from the members. Subject to concistency
  checks, of course.

  \par Example headers
If the scanner is known to stir::Scanner, you can use this
\verbatim
ROOT header := 
Originating system := Siemens mMR

; specify GATE output format (could be GATE_ECAT_PET as well)
GATE scanner type := GATE_Cylindrical_PET
GATE_Cylindrical_PET Parameters :=
  ; name of the actual ROOT file
  name of data file := mysim.root

  ; See elsewhere for other parameters
End GATE_Cylindrical_PET Parameters :=

end ROOT header := 
\endverbatim
  Below is an example using a user-defined scanner. 
\verbatim
ROOT header := 
Originating system := User_defined_scanner
Number of rings                          := 4
Number of detectors per ring             := 504
Inner ring diameter (cm)                 := 65.6
Average depth of interaction (cm)        := 0.7
Distance between rings (cm)              := 0.40625
Default bin size (cm)                    := 0.208626
Maximum number of non-arc-corrected bins := 344

GATE scanner type := GATE_Cylindrical_PET
GATE_Cylindrical_PET Parameters :=
  ; name of the actual ROOT file
  name of data file := mysim.root

  ; See elsewhere for other parameters
End GATE_Cylindrical_PET Parameters :=

end ROOT header := 
\endverbatim
*/
class CListModeDataROOT : public CListModeData
{
public:
    //! construct from the filename of the Interfile header
    CListModeDataROOT(const std::string& hroot_filename_prefix);

    //! returns the header filename
    virtual std::string
    get_name() const;
    //! Set private members default values;
    void set_defaults();

    virtual
    shared_ptr <CListRecord> get_empty_record_sptr() const;

    virtual
    Succeeded get_next_record(CListRecord& record) const;

    virtual
    Succeeded reset();

    virtual
    SavedPosition save_get_position();

    virtual
    Succeeded set_get_position(const SavedPosition&);

    virtual
    bool has_delayeds() const { return true; }

    virtual inline
    unsigned long int
    get_total_number_of_events() const ;

private:
    //! Check if the hroot contains a full scanner description
    Succeeded check_scanner_definition(std::string& ret);
    //! Check if the scanner_sptr matches the geometry in root_file_sptr
    Succeeded check_scanner_match_geometry(std::string& ret, const shared_ptr<Scanner>& scanner_sptr);

    //! The header file
    std::string hroot_filename;

    //! Pointer to the listmode data
    shared_ptr<InputStreamFromROOTFile > root_file_sptr;

//! \name Variables that can be set in the hroot file to define a scanner's geometry.
//! They are compared to the Scanner  (if set)  and the InputStreamFromROOTFile
//! geometry, as given by the repeaters. Can be used to check for inconsistencies.
//@{
    //! The name of the originating scanner
    std::string originating_system;
    //! Number of rings, set in the hroot file (optional)
    int num_rings;
    //! Number of detectors per ring, set in the hroot file (optional)
    int num_detectors_per_ring;
    //! Number of non arc corrected bins, set in the hroot file (optional)
    int max_num_non_arccorrected_bins;
    //! Inner ring diameter, set in the hroot file (optional)
    float inner_ring_diameter;
    //! Average depth of interaction, set in the hroot file (optional)
    float average_depth_of_interaction;
    //! Ring spacing, set in the hroot file (optional)
    float ring_spacing;
    //! Bin size, set in the hroot file (optional)
    float bin_size;
//@}

    KeyParser parser;

    Succeeded open_lm_file();
};


END_NAMESPACE_STIR

#endif
