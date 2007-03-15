/*=========================================================================

  Program:   ParaView
  Module:    vtkSelection.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSelection - A node in a selection tree. Used to store selection results.
// .SECTION Description

// vtkSelection is a node of a tree data structure used to store
// selection results. Each node in this tree stores a list of properties
// (in a vtkInformation) and a list of selection values (in a
// vtkAbstractArray). The properties provide information about what the
// selection values mean. For example the CONTENT_TYPE property gives
// information about what is stored by the node. If the CONTENT_TYPE is
// SELECTIONS, the node is used as a parent node that contains other
// vtkSelections and does not usually contain any selection values. If
// the CONTENT_TYPE is IDS, the SelectionList array should contain a
// list of ids. The type of id is specified by FIELD_TYPE (POINT or CELL).
// Usually, each node under the root is a selection from
// one data object. SOURCE or SOURCE_ID properties point to this object. If
// the selection was performed on a renderer, PROP or PROP_ID point to the
// prop the selection was made on. Selection nodes corresponding to
// composite datasets may contain child nodes. Each child node of a
// composite dataset should have GROUP and BLOCK set. This way, the pointer
// to the composite dataset can be obtained from the parent of a block
// node. The pointer to the block can be obtained from the composite
// dataset using GROUP and BLOCK.  
//
// .SECTION Caveats 
// Each node can have one parent and should not be added to more than one
// node as child. No SelectionList is created by default. It should be assigned.

#ifndef __vtkSelection_h
#define __vtkSelection_h

#include "vtkDataObject.h"

//BTX
class vtkAbstractArray;
class vtkInformation;
class vtkInformationIntegerKey;
class vtkInformationObjectBaseKey;
struct vtkSelectionInternals;
//ETX

class VTK_FILTERING_EXPORT vtkSelection : public vtkDataObject
{
public:
  vtkTypeRevisionMacro(vtkSelection,vtkDataObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  static vtkSelection* New();

  // Description:
  // Sets the selection list.
  virtual void SetSelectionList(vtkAbstractArray*);

  // Description:
  // Returns the selection list.
  vtkGetObjectMacro(SelectionList, vtkAbstractArray);

  // Description:
  // Returns the property map.
  vtkGetObjectMacro(Properties, vtkInformation);

  // Description: 
  // Returns the number of children.
  unsigned int GetNumberOfChildren();

  // Description: 
  // Returns a child given it's index. Performs bound checking
  // and will return 0 if out-of-bounds.
  virtual vtkSelection* GetChild(unsigned int idx);

  // Description: 
  // Returns the parent of the selection node unless it is root.
  // A child does not keep a reference to the parent to avoid
  // reference loops.
  virtual vtkSelection* GetParentNode()
    {
      return this->ParentNode;
    }

  // Description: 
  // Adds a child node. If the node is already a child, it is not
  // added a second time. Note that a node can be a child of only
  // one node at a time. This method will also set the parent of
  // the passed node to this.
  virtual void AddChild(vtkSelection*);

  // Description: 
  // Removes a child. It will also set the parent of the removed
  // child to be 0.
  virtual void RemoveChild(unsigned int idx);
  virtual void RemoveChild(vtkSelection*);

  // Description: 
  // Removes all properties and children. Removes selection list array.
  // Does not change parent node.
  virtual void Clear();

  // Description: 
  // Copy properties, selection list and children of the input.
  virtual void DeepCopy(vtkDataObject* src);

  // Description: 
  // Add the children of the given selection to this one. 
  // This requires that both selection objects have a SELECTIONS
  // CONTENT_TYPE. Note that this does not check if a child with
  // exact same properties exists before adding. If any child node
  // that contains other selections is found in the input, it's
  // children are added to a selection node of the same SOURCE_ID
  // or PROP_ID. This handles the case of assemblies and composite
  // datasets.
  virtual void CopyChildren(vtkSelection*);

  // Description:
  // Return the MTime taking into account changes to the properties
  unsigned long GetMTime();

  // vtkSelection specific keys.

  // Description:
  // The content of the selection node. See SelectionContent
  // enum for the possible values.
  static vtkInformationIntegerKey* CONTENT_TYPE();

//BTX
  enum SelectionContent
  {
    SELECTIONS,
    COMPOSITE_SELECTIONS,
    IDS,
    ID_RANGE,
    FRUSTUM,
    LOCATIONS,
    THRESHOLDS
  };
//ETX

  // Description:
  // The location of the array the selection came from (ex, point, cell or field)
  static vtkInformationIntegerKey* FIELD_TYPE();

//BTX
  enum SelectionField
  {
    POINT,
    CELL
  };
//ETX

  // Description:
  // Pointer to the data or algorithm the selection belongs to.
  static vtkInformationObjectBaseKey* SOURCE();

  // Description:
  // ID of the data or algorithm the selection belongs to. What
  // ID means is application specific.
  static vtkInformationIntegerKey* SOURCE_ID();

  // Description:
  // Pointer to the prop the selection belongs to.
  static vtkInformationObjectBaseKey* PROP();

  // Description:
  // ID of the prop the selection belongs to. What
  // ID means is application specific.
  static vtkInformationIntegerKey* PROP_ID();

  // Description:
  // Process id the selection is on.
  static vtkInformationIntegerKey* PROCESS_ID();

  // Description:
  // The composite data group the selection belongs to.
  static vtkInformationIntegerKey* GROUP();

  // Description:
  // The composite data block the selection belongs to.
  static vtkInformationIntegerKey* BLOCK();

  // Description:
  // The name of the array the selection came from.
  static vtkInformationStringKey* ARRAY_NAME();

  // Description:
  // For location selection of points, if distance is greater than this reject.
  static vtkInformationDoubleKey* EPSILON();

  // Description:
  // This flag tells the extraction filter not to convert the selected
  // output into an unstructured grid, but instead to produce a vtkInsidedness
  // array and add it to the input dataset. 
  // Warning: this is only supported by vtkExtractSelectedFrustum for now.
  static vtkInformationIntegerKey* PRESERVE_TOPOLOGY();

  // Description:
  // This flag tells the extraction filter, when FIELD_TYPE==POINT, that
  // it should also extract the cells that contain any of the extracted points.
  // Warning: this is not yet supported. Some extractions act as if
  // this is on and some as if it is off.
  static vtkInformationIntegerKey* CONTAINING_CELLS();

  // Description:
  // Retrieve a selection from an information vector.
  static vtkSelection* GetData(vtkInformation* info);
  static vtkSelection* GetData(vtkInformationVector* v, int i=0);

protected:
  vtkSelection();
  ~vtkSelection();

  vtkInformation* Properties;
  vtkAbstractArray* SelectionList;
  vtkSelection* ParentNode;

private:
  vtkSelection(const vtkSelection&);  // Not implemented.
  void operator=(const vtkSelection&);  // Not implemented.

  vtkSelectionInternals* Internal;
};

#endif
