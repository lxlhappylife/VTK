/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGL2PSExporter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkGL2PSExporter - export a scene as a PostScript file using GL2PS.
// .SECTION Description
// vtkGL2PSExporter is a concrete subclass of vtkExporter that writes
// high quality vector PostScript (PS/EPS), PDF or SVG files by using
// GL2PS.  GL2PS can be obtained at: http://www.geuz.org/gl2ps/.  This
// can be very useful when one requires publication quality pictures.
// This class works best with simple 3D scenes and most 2D plots.
// Please note that GL2PS has its limitations since PostScript is not
// an ideal language to represent complex 3D scenes.  However, this
// class does allow one to write mixed vector/raster files by using
// the Write3DPropsAsRasterImage ivar.  Please do read the caveats
// section of this documentation.
//
// By default vtkGL2PSExporter generates Encapsulated PostScript (EPS)
// output along with the text in portrait orientation with the
// background color of the window being drawn.  The generated output
// is also compressed using zlib. The various other options are set to
// sensible defaults.
//
// The output file format (FileFormat) can be either PostScript (PS),
// Encapsulated PostScript (EPS), PDF, SVG or TeX.  The file extension
// is generated automatically depending on the FileFormat.  The
// default is EPS.  When TeX output is chosen, only the text strings
// in the plot are generated and put into a picture environment.  One
// can turn on and off the text when generating PS/EPS/PDF/SVG files
// by using the Text boolean variable.  By default the text is drawn.
// The background color of the renderwindow is drawn by default.  To
// make the background white instead use the DrawBackgroundOff
// function.  Landscape figures can be generated by using the
// LandscapeOn function.  Portrait orientation is used by default.
// Several of the GL2PS options can be set.  The names of the ivars
// for these options are similar to the ones that GL2PS provides.
// Compress, SimpleLineOffset, Silent, BestRoot, PS3Shading and
// OcclusionCull are similar to the options provided by GL2PS.  Please
// read the function documentation or the GL2PS documentation for more
// details.  The ivar Write3DPropsAsRasterImage allows one to generate
// mixed vector/raster images.  All the 3D props in the scene will be
// written as a raster image and all 2D actors will be written as
// vector graphic primitives.  This makes it possible to handle
// transparency and complex 3D scenes.  This ivar is set to Off by
// default.  Specific 3D props can be excluded from the rasterization
// process by adding them to the RasterExclusions ivar.  Props in this
// collection will be rendered as 2D vector primitives instead. When
// drawing lines and points the OpenGL point size and
// line width are multiplied by a factor in order to generate
// PostScript lines and points of the right size.  The
// Get/SetGlobalPointSizeFactor and Get/SetGlobalLineWidthFactor let
// one customize this ratio.  The default value is such that the
// PostScript output looks close to what is seen on screen.
//
// To use this class you need to turn on VTK_USE_GL2PS when
// configuring VTK.

// .SECTION Caveats
// By default (with Write3DPropsAsRasterImage set to Off) exporting
// complex 3D scenes can take a long while and result in huge output
// files.  Generating correct vector graphics output for scenes with
// transparency is almost impossible.  However, one can set
// Write3DPropsAsRasterImageOn and generate mixed vector/raster files.
// This should work fine with complex scenes along with transparent
// actors.

// .SECTION See Also
// vtkExporter

// .SECTION Thanks
// Thanks to Goodwin Lawlor and Prabhu Ramachandran for this class.


#ifndef __vtkGL2PSExporter_h
#define __vtkGL2PSExporter_h

#include "vtkIOExportModule.h" // For export macro
#include "vtkExporter.h"

class vtkActor2D;
class vtkCollection;
class vtkCoordinate;
class vtkIntArray;
class vtkMathTextActor;
class vtkMathTextActor3D;
class vtkMatrix4x4;
class vtkPath;
class vtkProp;
class vtkPropCollection;
class vtkProp3DCollection;
class vtkRenderer;
class vtkRendererCollection;
class vtkTextActor;
class vtkTextActor3D;
class vtkTextMapper;
class vtkTextProperty;

class VTKIOEXPORT_EXPORT vtkGL2PSExporter : public vtkExporter
{
public:
  static vtkGL2PSExporter *New();
  vtkTypeMacro(vtkGL2PSExporter,vtkExporter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Specify the prefix of the files to write out. The resulting filenames
  // will have .ps or .eps or .tex appended to them depending on the
  // other options chosen.
  vtkSetStringMacro(FilePrefix);
  vtkGetStringMacro(FilePrefix);

  // Description:
  // Set the title for the output, if supported. If NULL, "VTK GL2PS Export" is
  // used.
  vtkSetStringMacro(Title);
  vtkGetStringMacro(Title);

//BTX
  enum OutputFormat
  {
      PS_FILE,
      EPS_FILE,
      PDF_FILE,
      TEX_FILE,
      SVG_FILE
  };
//ETX

  // Description:
  // Specify the format of file to write out.  This can be one of:
  // PS_FILE, EPS_FILE, PDF_FILE, TEX_FILE.  Defaults to EPS_FILE.
  // Depending on the option chosen it generates the appropriate file
  // (with correct extension) when the Write function is called.
  vtkSetClampMacro(FileFormat, int, PS_FILE, SVG_FILE);
  vtkGetMacro(FileFormat, int);
  void SetFileFormatToPS()
    {this->SetFileFormat(PS_FILE);};
  void SetFileFormatToEPS()
    {this->SetFileFormat(EPS_FILE);};
  void SetFileFormatToPDF()
    {this->SetFileFormat(PDF_FILE);};
  void SetFileFormatToTeX()
    {this->SetFileFormat(TEX_FILE);};
  void SetFileFormatToSVG()
    {this->SetFileFormat(SVG_FILE);};
  const char *GetFileFormatAsString();

//BTX
  enum SortScheme
  {
      NO_SORT=0,
      SIMPLE_SORT=1,
      BSP_SORT=2
  };
//ETX

  // Description:
  // Set the the type of sorting algorithm to order primitives from
  // back to front.  Successive algorithms are more memory
  // intensive.  Simple is the default but BSP is perhaps the best.
  vtkSetClampMacro(Sort, int, NO_SORT, BSP_SORT);
  vtkGetMacro(Sort,int);
  void SetSortToOff()
    {this->SetSort(NO_SORT);};
  void SetSortToSimple()
    {this->SetSort(SIMPLE_SORT);};
  void SetSortToBSP()
    {this->SetSort(BSP_SORT);};
  const char *GetSortAsString();

  // Description:
  // Turn on/off compression when generating PostScript or PDF
  // output. By default compression is on.
  vtkSetMacro(Compress, int);
  vtkGetMacro(Compress, int);
  vtkBooleanMacro(Compress, int);

  // Description:
  // Turn on/off drawing the background frame.  If off the background
  // is treated as white.  By default the background is drawn.
  vtkSetMacro(DrawBackground, int);
  vtkGetMacro(DrawBackground, int);
  vtkBooleanMacro(DrawBackground, int);

  // Description:
  // Turn on/off the GL2PS_SIMPLE_LINE_OFFSET option.  When enabled a
  // small offset is added in the z-buffer to all the lines in the
  // plot.  This results in an anti-aliasing like solution.  Defaults to
  // on.
  vtkSetMacro(SimpleLineOffset, int);
  vtkGetMacro(SimpleLineOffset, int);
  vtkBooleanMacro(SimpleLineOffset, int);

  // Description:
  // Turn on/off GL2PS messages sent to stderr (GL2PS_SILENT).  When
  // enabled GL2PS messages are suppressed.  Defaults to off.
  vtkSetMacro(Silent, int);
  vtkGetMacro(Silent, int);
  vtkBooleanMacro(Silent, int);

  // Description:
  // Turn on/off the GL2PS_BEST_ROOT option.  When enabled the
  // construction of the BSP tree is optimized by choosing the root
  // primitives leading to the minimum number of splits.  Defaults to
  // on.
  vtkSetMacro(BestRoot, int);
  vtkGetMacro(BestRoot, int);
  vtkBooleanMacro(BestRoot, int);

  // Description:
  // Turn on/off drawing the text.  If on (default) the text is drawn.
  // If the FileFormat is set to TeX output then a LaTeX picture is
  // generated with the text strings.  If off text output is
  // suppressed.
  vtkSetMacro(Text, int);
  vtkGetMacro(Text, int);
  vtkBooleanMacro(Text, int);

  // Description:
  // Turn on/off landscape orientation.  If off (default) the
  // orientation is set to portrait.
  vtkSetMacro(Landscape, int);
  vtkGetMacro(Landscape, int);
  vtkBooleanMacro(Landscape, int);

  // Description:
  // Turn on/off the GL2PS_PS3_SHADING option.  When enabled the
  // shfill PostScript level 3 operator is used.  Read the GL2PS
  // documentation for more details.  Defaults to on.
  vtkSetMacro(PS3Shading, int);
  vtkGetMacro(PS3Shading, int);
  vtkBooleanMacro(PS3Shading, int);

  // Description:
  // Turn on/off culling of occluded polygons (GL2PS_OCCLUSION_CULL).
  // When enabled hidden polygons are removed.  This reduces file size
  // considerably.  Defaults to on.
  vtkSetMacro(OcclusionCull, int);
  vtkGetMacro(OcclusionCull, int);
  vtkBooleanMacro(OcclusionCull, int);

  // Description:
  // Turn on/off writing 3D props as raster images.  2D props are
  // rendered using vector graphics primitives.  If you have hi-res
  // actors and are using transparency you probably need to turn this
  // on.  Defaults to Off.
  vtkSetMacro(Write3DPropsAsRasterImage, int);
  vtkGetMacro(Write3DPropsAsRasterImage, int);
  vtkBooleanMacro(Write3DPropsAsRasterImage, int);

  // Description:
  // Collection of 3D Props to exclude from rasterization. These will be
  // rendered as 2D vector primitives in the output. This setting is ignored if
  // Write3DPropsAsRasterImage is false. Behind the scenes, these props are
  // treated as 2D props during the vector output generation.
  void SetRasterExclusions(vtkProp3DCollection*);
  vtkGetObjectMacro(RasterExclusions, vtkProp3DCollection);

protected:
  vtkGL2PSExporter();
  ~vtkGL2PSExporter();

  void WriteData();

  int GetGL2PSOptions();
  int GetGL2PSSort();
  int GetGL2PSFormat();
  const char *GetFileExtension();

  void SavePropVisibility(vtkRendererCollection *renCol,
                          vtkIntArray *volVis, vtkIntArray *actVis,
                          vtkIntArray *act2dVis);
  void RestorePropVisibility(vtkRendererCollection *renCol,
                             vtkIntArray *volVis, vtkIntArray *actVis,
                             vtkIntArray *act2dVis);
  void Turn3DPropsOff(vtkRendererCollection *renCol);
  void Turn2DPropsOff(vtkRendererCollection *renCol);
  void GetVisibleContextActors(vtkPropCollection *contextActors,
                               vtkRendererCollection *renCol);
  void SetPropVisibilities(vtkPropCollection *col, int vis);

  void DrawSpecialProps(vtkCollection *propCol, vtkRendererCollection *renCol);
  // Description:
  // Reimplement this to handle your own special props. Must call this function
  // at the end of the override for default handling.
  virtual void HandleSpecialProp(vtkProp *prop, vtkRenderer *ren);
  void DrawTextActor(vtkTextActor *textAct, vtkRenderer *ren);
  void DrawTextActor3D(vtkTextActor3D *textAct, vtkRenderer *ren);
  void DrawTextMapper(vtkTextMapper *textMap, vtkActor2D *textAct,
                      vtkRenderer *ren);
  void DrawMathTextActor(vtkMathTextActor *textAct,
                         vtkRenderer *ren);
  void DrawMathTextActor3D(vtkMathTextActor3D *textAct,
                           vtkRenderer *ren);
  void DrawViewportTextOverlay(const char *string, vtkTextProperty *tprop,
                               vtkCoordinate *coord, vtkRenderer *ren);
  // Description:
  // Transform the path using the actor's matrix and current GL state, then
  // draw it to GL2PS.
  void Draw3DPath(vtkPath *path, vtkMatrix4x4 *actorMatrix,
                  double actorBounds[4], unsigned char actorColor[3]);
  // Description:
  // Copy the region copyRect from the framebuffer into the gl2ps document.
  // copyRect is in viewport coordinates [xmin, ymin, width, height].
  void CopyPixels(int copyRect[4], vtkRenderer *ren);

  void DrawContextActors(vtkPropCollection *contextActs,
                         vtkRendererCollection *renCol);


  vtkProp3DCollection *RasterExclusions;

  char *FilePrefix;
  char *Title;
  int FileFormat;
  int Sort;
  int Compress;
  int DrawBackground;
  int SimpleLineOffset;
  int Silent;
  int BestRoot;
  int Text;
  int Landscape;
  int PS3Shading;
  int OcclusionCull;
  int Write3DPropsAsRasterImage;

  float *PixelData;
  int PixelDataSize[2];

private:
  vtkGL2PSExporter(const vtkGL2PSExporter&); // Not implemented
  void operator=(const vtkGL2PSExporter&); // Not implemented
};

inline const char *vtkGL2PSExporter::GetSortAsString(void)
{
  if ( this->Sort == NO_SORT )
    {
    return "Off";
    }
  else if ( this->Sort == SIMPLE_SORT )
    {
    return "Simple";
    }
  else
    {
    return "BSP";
    }
}

inline const char *vtkGL2PSExporter::GetFileFormatAsString(void)
{
  if ( this->FileFormat == PS_FILE )
    {
    return "PS";
    }
  else if ( this->FileFormat == EPS_FILE )
    {
    return "EPS";
    }
  else if ( this->FileFormat == PDF_FILE )
    {
    return "PDF";
    }
  else if ( this->FileFormat == TEX_FILE )
    {
    return "TeX";
    }
  else
    {
    return "SVG";
    }
}

#endif
