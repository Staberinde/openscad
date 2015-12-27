#pragma once

#include <vector>
#include <string>
#include "traverser.h"

extern int progress_report_count;
extern void (*progress_report_f)(const class AbstractNode*, void*, int);
extern void *progress_report_vp;

void progress_report_prep(AbstractNode *root, void (*f)(const class AbstractNode *node, void *vp, int mark), void *vp);
void progress_report_fin();

/*!  

	The node tree is the result of evaluation of a module instantiation
	tree.  Both the module tree and the node tree are regenerated from
	scratch for each compile.

 */
class AbstractNode
{
	// FIXME: the idx_counter/idx is mostly (only?) for debugging.
	// We can hash on pointer value or smth. else.
  //  -> remove and
	// use smth. else to display node identifier in CSG tree output?
	static size_t idx_counter;   // Node instantiation index
public:
	AbstractNode(const class ModuleInstantiation *mi);
	virtual ~AbstractNode();
  virtual Response accept(class State &state, class Visitor &visitor) const;
	virtual std::string toString() const;
	/*! The 'OpenSCAD name' of this node, defaults to classname, but can be 
	    overloaded to provide specialization for e.g. CSG nodes, primitive nodes etc.
	    Used for human-readable output. */
	virtual std::string name() const = 0;
  /*! Should return a Geometry instance describing the node. Returns NULL if smth.
		goes wrong. This is only called by PolySetEvaluator, to make sure polysets 
		are inserted into the cache*/
	virtual class Geometry *evaluate_geometry(class PolySetEvaluator *) const { return NULL; }

	const std::vector<AbstractNode*> &getChildren() const { 
		return this->children;
	}
	size_t index() const { return this->idx; }

	static void resetIndexCounter() { idx_counter = 1; }

	// FIXME: Make protected
	std::vector<AbstractNode*> children;
	const ModuleInstantiation *modinst;

	// progress_mark is a running number used for progress indication
	// FIXME: Make all progress handling external, put it in the traverser class?
	int progress_mark;
	void progress_prepare();
	void progress_report() const;

	int idx; // Node index (unique per tree)
};

class AbstractIntersectionNode : public AbstractNode
{
public:
	AbstractIntersectionNode(const ModuleInstantiation *mi) : AbstractNode(mi) { };
	virtual ~AbstractIntersectionNode() { };
  virtual Response accept(class State &state, class Visitor &visitor) const;
	virtual std::string toString() const;
	virtual std::string name() const;
};

class AbstractPolyNode : public AbstractNode
{
public:
	AbstractPolyNode(const ModuleInstantiation *mi) : AbstractNode(mi) { };
	virtual ~AbstractPolyNode() { };
  virtual Response accept(class State &state, class Visitor &visitor) const;

	enum render_mode_e {
		RENDER_CGAL,
		RENDER_OPENCSG
	};
};

/*!
<<<<<<< HEAD
	Used for organizing objects into lists which should not be grouped but merely
	unpacked by the parent node.
 */
class ListNode : public AbstractNode
{
public:
	ListNode(const class ModuleInstantiation *mi) : AbstractNode(mi) { }
	virtual ~ListNode() { }
  virtual Response accept(class State &state, class Visitor &visitor) const;
	virtual std::string name() const;
};

/*!
=======
>>>>>>> origin/master
  Logically groups objects together. Used as a way of passing
	objects around without having to perform unions on them.
 */
class GroupNode : public AbstractNode
{
public:
	GroupNode(const class ModuleInstantiation *mi) : AbstractNode(mi) { }
	virtual ~GroupNode() { }
  virtual Response accept(class State &state, class Visitor &visitor) const;
	virtual std::string name() const;
};

/*!
	Only instantiated once, for the top-level file.
*/
class RootNode : public GroupNode
{
public:
	RootNode(const class ModuleInstantiation *mi) : GroupNode(mi) { }
	virtual ~RootNode() { }
  virtual Response accept(class State &state, class Visitor &visitor) const;
	virtual std::string name() const;
};

class LeafNode : public AbstractPolyNode
{
public:
	LeafNode(const ModuleInstantiation *mi) : AbstractPolyNode(mi) { };
	virtual ~LeafNode() { };
  virtual Response accept(class State &state, class Visitor &visitor) const;
	virtual class Geometry *createGeometry() const = 0;
};

std::ostream &operator<<(std::ostream &stream, const AbstractNode &node);
AbstractNode *find_root_tag(AbstractNode *n);
