/*                                                

  Author(s): Simon Leonard
  Created on: Nov 11 2009 

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.                                                    

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _robGeomBase_h
#define _robGeomBase_h

#include <string>
#include <iostream>

#include <cisstVector/vctMatrixRotation3.h>
#include <cisstVector/vctFrame4x4.h>

#include <cisstRobot/robDefines.h>
#include <cisstRobot/robExport.h>

//! Base class for geometry
/**
   robGeomBase defines the interface for geometries. The primary purpose of this
   interface is graphics rendering in OpenGL. Thus, any subclass, must implement
   a Draw() method to draw the geometry in OpenGL. robGeomBase contains a 
   vctFrame4x4 and that is used to store the position and orientation of the 
   geometry's coordinate frame.
*/
class CISST_EXPORT robGeomBase {

protected:

  //! The position and orientation of the geometry's coordinate frame
  vctFrame4x4<double,VCT_ROW_MAJOR> Rt;

  //! Multiply the OpenGL current matrix with the specified matrix
  /**
     For OpenGL. Call this before drawing your geometry to push the position
     and orientation on the stack and call this (with the inverse) to remove
     the orientation and position.
     \param Rt The position and orientation of the geometry
  */
  void glMultMatrix( const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt ) const;

public:

  //! Default constructor
  robGeomBase();

  //! Default destructor
  virtual ~robGeomBase();

  //! Read a file name from a input stream and load the file
  /**
     Read a file name from the input stream and then call Load to load the 
     geometry into memory. The implementation of Load is left to a subclass.
  */
  virtual robError Read( std::istream& is );

  //! Load a file
  /**
     Pure virtual function. This method is called to load a geometry into memory.
     The type of file and geometry representation is implementation specific.
     \param filename The file name of the file to load
     \return SUCCESS if the file was loaded properly. ERROR otherwise.
   */
  virtual robError Load( const std::string& filename ) = 0;
  
  //! Render the geometry
  /**
     This method is called automatically to draw the geometry in OpenGL. If you 
     want to render your geometry in OpenGL, then you need to make a subclass and
     overload Draw(). If you don't need to draw the geometry, then leave this 
     method empty.
     \warning Not thread safe: The transformation Rt can be overwritten during
              rendering.
  */
  virtual void Draw() const = 0;

  //! Set the position of the geometry
  /**
     Set the position of the geometry's coordinate frame
     \param t A 3D position vector
  */
  virtual void SetGeomPosition( const vctFixedSizeVector<double,3>& t);

  //! Set the orientation of the geometry
  /**
     Set the orientation of the geometry's coordinate frame
     \param t A 3D rotation matrix
  */
  virtual void SetGeomOrientation(const vctMatrixRotation3<double,VCT_ROW_MAJOR>& R);
  
  //! Return the position of the geometry
  /**
     \return The position of the geometry's coordinate frame
  */
  virtual vctFixedSizeVector<double,3> GetGeomPosition() const;
  
  //! Return the orientation of the geometry
  /**
     \return The orientation of the geometry's coordinate frame
  */
  virtual vctMatrixRotation3<double,VCT_ROW_MAJOR> GetGeomOrientation() const;

};

#endif