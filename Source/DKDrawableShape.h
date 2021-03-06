/**
 @author Contributions from the community; see CONTRIBUTORS.md
 @date 2005-2016
 @copyright MPL2; see LICENSE.txt
*/

#import "DKDrawableObject.h"

@class DKDrawablePath, DKDistortionTransform, DKGridLayer;

// edit operation constants tell the shape what info to display in the floater

typedef enum {
	kDKShapeOperationResize = 0,
	kDKShapeOperationMove = 1,
	kDKShapeOperationRotate = 2
} DKShapeEditOperation;

/** @brief A semi-abstract base class for a drawable object consisting of any path-based shape that can be drawn to fit a rectangle.

DKDrawableShape is a semi-abstract base class for a drawable object consisting of any path-based shape that can be drawn to fit a
rectangle. DKShapeFactory can be used to supply lots of different path shapes usable with this class.

This implements rotation of the shape about a specified point (defaulting to the centre), and also standard selection handles.

Resizing and moving of the shape is handled by its implementations of the mouseDown/dragged/up event handlers.

This class uses the handle drawing supplied by DKKnob.

The path is stored internally in its untransformed form. This means that its datum is at the origin and it is unrotated. When rendered, the
object's location and rotation angle are applied so what you see is what you expect. The bounds naturally refers to the transformed
bounds. The selection handles themselves are also transformed by the same transform, since the shape remains editable at any orientation.

The canonical path is set to have a bounding rect 1.0 on each side. The actual size of the object is factored into the transform to
render the object in the drawing. Thus the original path is NEVER changed once established. This allows us to share basic shapes which
can be generated by a factory class.
*/
@interface DKDrawableShape : DKDrawableObject <NSCoding, NSCopying, NSDraggingDestination> {
@private
	NSBezierPath* m_path; // shape's path stored in canonical form (origin centred and with unit size)
	NSMutableArray* m_customHotSpots; // list of attached custom hotspots (if any)
	DKDistortionTransform* m_distortTransform; // distortion transform for distort operations
	CGFloat m_rotationAngle; // angle of rotation of the shape
	NSPoint m_location; // where in the drawing it is placed
	NSSize m_scale; // object size/scale
	NSSize m_offset; // offset from origin of logical centre relative to canonical path
	BOOL m_hideOriginTarget; // YES to hide temporarily the origin target - done for some mouse operations
	NSInteger m_opMode; // drag operation mode - normal versus distortion modes
@protected
	NSRect mBoundsCache; // cached value of the bounds
	BOOL m_inRotateOp; // YES while a rotation drag is in progress
}

/** @brief Return which particular knobs are used by instances of this class

 The default is to use all knobs, but subclasses may want to override this for particular uses
 @return bitmask indicating which knobs are used
 */
+ (NSInteger)knobMask;
+ (void)setKnobMask:(NSInteger)knobMask;
+ (void)setAngularConstraintAngle:(CGFloat)radians;
+ (NSRect)unitRectAtOrigin;
+ (void)setInfoWindowBackgroundColour:(NSColor*)colour;
+ (NSCursor*)cursorForShapePartcode:(NSInteger)pc;

// convenient ways to create shapes for a path you have:

+ (DKDrawableShape*)drawableShapeWithRect:(NSRect)aRect;
+ (DKDrawableShape*)drawableShapeWithOvalInRect:(NSRect)aRect;
+ (DKDrawableShape*)drawableShapeWithCanonicalBezierPath:(NSBezierPath*)path;
+ (DKDrawableShape*)drawableShapeWithBezierPath:(NSBezierPath*)path;
+ (DKDrawableShape*)drawableShapeWithBezierPath:(NSBezierPath*)path rotatedToAngle:(CGFloat)angle;
+ (DKDrawableShape*)drawableShapeWithBezierPath:(NSBezierPath*)path withStyle:(DKStyle*)aStyle;
+ (DKDrawableShape*)drawableShapeWithBezierPath:(NSBezierPath*)path rotatedToAngle:(CGFloat)angle withStyle:(DKStyle*)aStyle;

// initialise a shape for a rect or oval or different kinds of path:

- (id)initWithRect:(NSRect)aRect;
- (id)initWithOvalInRect:(NSRect)aRect;
- (id)initWithCanonicalBezierPath:(NSBezierPath*)aPath;
- (id)initWithBezierPath:(NSBezierPath*)aPath;
- (id)initWithBezierPath:(NSBezierPath*)aPath rotatedToAngle:(CGFloat)angle;

// allowing style to be passed directly:

- (id)initWithRect:(NSRect)aRect style:(DKStyle*)aStyle;
- (id)initWithOvalInRect:(NSRect)aRect style:(DKStyle*)aStyle;
- (id)initWithCanonicalBezierPath:(NSBezierPath*)aPath style:(DKStyle*)aStyle;
- (id)initWithBezierPath:(NSBezierPath*)aPath style:(DKStyle*)aStyle;
- (id)initWithBezierPath:(NSBezierPath*)aPath rotatedToAngle:(CGFloat)angle style:(DKStyle*)style;

// path operations:

- (void)setPath:(NSBezierPath*)path;
- (NSBezierPath*)path;
- (void)reshapePath;
- (void)adoptPath:(NSBezierPath*)path;
- (NSBezierPath*)transformedPath;
- (BOOL)canPastePathWithPasteboard:(NSPasteboard*)pb;

// geometry:

- (NSAffineTransform*)transform;
- (NSAffineTransform*)transformIncludingParent;
- (NSAffineTransform*)inverseTransform;
- (NSPoint)locationIgnoringOffset;

- (void)rotateUsingReferencePoint:(NSPoint)rp constrain:(BOOL)constrain;
- (void)moveKnob:(NSInteger)knobPartCode toPoint:(NSPoint)p allowRotate:(BOOL)rotate constrain:(BOOL)constrain;
- (void)setDragAnchorToPart:(NSInteger)knobPartCode;

- (void)flipHorizontally;
- (void)flipVertically;

- (void)resetBoundingBox;
- (void)resetBoundingBoxAndRotation;
- (void)adjustToFitGrid:(DKGridLayer*)grid;

- (BOOL)allowSizeKnobsToRotateShape;

// operation modes:

- (void)setOperationMode:(NSInteger)mode;
- (NSInteger)operationMode;

// knob and partcode methods

- (NSString*)undoActionNameForPartCode:(NSInteger)pc;
- (NSPoint)knobPoint:(NSInteger)knobPartCode;
- (NSPoint)rotationKnobPoint;
- (void)drawKnob:(NSInteger)knobPartCode;
- (NSPoint)convertPointFromRelativeLocation:(NSPoint)rloc;

// distortion ops:

- (void)setDistortionTransform:(DKDistortionTransform*)dt;
- (DKDistortionTransform*)distortionTransform;

// convert to editable path:

- (DKDrawablePath*)makePath;
- (NSArray*)breakApart;

// user actions:

- (IBAction)convertToPath:(id)sender;
- (IBAction)unrotate:(id)sender;
- (IBAction)rotate:(id)sender;
- (IBAction)setDistortMode:(id)sender;
- (IBAction)resetBoundingBox:(id)sender;
- (IBAction)toggleHorizontalFlip:(id)sender;
- (IBAction)toggleVerticalFlip:(id)sender;
- (IBAction)pastePath:(id)sender;

@end

// part codes:

// since part codes are private to each drawable class (except 0 meaning no part), these are arranged partially as
// bit values, so they can be added together to indicate corner positions. Don't change these numbers as this
// is taken advantage of internally to simplify the handling of the part codes.

enum {
	kDKDrawableShapeLeftHandle = (1 << 0),
	kDKDrawableShapeTopHandle = (1 << 1),
	kDKDrawableShapeRightHandle = (1 << 2),
	kDKDrawableShapeBottomHandle = (1 << 3),
	kDKDrawableShapeTopLeftHandle = (1 << 4),
	kDKDrawableShapeTopRightHandle = (1 << 5),
	kDKDrawableShapeBottomLeftHandle = (1 << 6),
	kDKDrawableShapeBottomRightHandle = (1 << 7),
	kDKDrawableShapeObjectCentre = (1 << 8),
	kDKDrawableShapeOriginTarget = (1 << 9),
	kDKDrawableShapeRotationHandle = (1 << 10),
	kDKDrawableShapeTopLeftDistort = (1 << 11),
	kDKDrawableShapeTopRightDistort = (1 << 12),
	kDKDrawableShapeBottomRightDistort = (1 << 13),
	kDKDrawableShapeBottomLeftDistort = (1 << 14),
	kDKDrawableShapeSnapToPathEdge = -98,
};

// knob masks:

enum {
	kDKDrawableShapeAllKnobs = 0xFFFFFFFF,
	kDKDrawableShapeAllSizeKnobs = kDKDrawableShapeAllKnobs & ~(kDKDrawableShapeRotationHandle | kDKDrawableShapeOriginTarget | kDKDrawableShapeObjectCentre),
	kDKDrawableShapeHorizontalSizingKnobs = (kDKDrawableShapeLeftHandle | kDKDrawableShapeRightHandle | kDKDrawableShapeTopLeftHandle | kDKDrawableShapeTopRightHandle | kDKDrawableShapeBottomLeftHandle | kDKDrawableShapeBottomRightHandle),
	kDKDrawableShapeVerticalSizingKnobs = (kDKDrawableShapeTopHandle | kDKDrawableShapeBottomHandle | kDKDrawableShapeTopLeftHandle | kDKDrawableShapeTopRightHandle | kDKDrawableShapeBottomLeftHandle | kDKDrawableShapeBottomRightHandle),
	kDKDrawableShapeAllLeftHandles = (kDKDrawableShapeLeftHandle | kDKDrawableShapeTopLeftHandle | kDKDrawableShapeBottomLeftHandle),
	kDKDrawableShapeAllRightHandles = (kDKDrawableShapeRightHandle | kDKDrawableShapeTopRightHandle | kDKDrawableShapeBottomRightHandle),
	kDKDrawableShapeAllTopHandles = (kDKDrawableShapeTopHandle | kDKDrawableShapeTopLeftHandle | kDKDrawableShapeTopRightHandle),
	kDKDrawableShapeAllBottomHandles = (kDKDrawableShapeBottomHandle | kDKDrawableShapeBottomLeftHandle | kDKDrawableShapeBottomRightHandle),
	kDKDrawableShapeAllCornerHandles = (kDKDrawableShapeTopLeftHandle | kDKDrawableShapeTopRightHandle | kDKDrawableShapeBottomLeftHandle | kDKDrawableShapeBottomRightHandle),
	kDKDrawableShapeNWSECorners = (kDKDrawableShapeTopLeftHandle | kDKDrawableShapeBottomRightHandle),
	kDKDrawableShapeNESWCorners = (kDKDrawableShapeBottomLeftHandle | kDKDrawableShapeTopRightHandle),
	kDKDrawableShapeEWHandles = (kDKDrawableShapeLeftHandle | kDKDrawableShapeRightHandle),
	kDKDrawableShapeNSHandles = (kDKDrawableShapeTopHandle | kDKDrawableShapeBottomHandle)
};

// operation modes:

enum {
	kDKShapeTransformStandard = 0, // normal resize/rotate transforms
	kDKShapeTransformFreeDistort = 1, // free distort transform
	kDKShapeTransformHorizontalShear = 2, // shear horizontally
	kDKShapeTransformVerticalShear = 3, // shear vertically
	kDKShapeTransformPerspective = 4 // perspective
};
