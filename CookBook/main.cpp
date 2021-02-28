#include <osgDB/ReadFile>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osgViewer/Viewer>
#include <osgText/Font>
#include <osgText/Text>
#include <osg/Camera>

class osgCookBook{
public:
	osg::Camera* createHUDCamera( double left, double right, double bottom, double top )
	{
		osg::ref_ptr<osg::Camera> camera = new osg::Camera;
		camera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
		camera->setClearMask( GL_DEPTH_BUFFER_BIT );
		camera->setRenderOrder( osg::Camera::POST_RENDER );
		camera->setAllowEventFocus( false );
		camera->setProjectionMatrix(osg::Matrix::ortho2D(left, right, bottom, top) );
		camera->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF );
		return camera.release();
	}

	osg::ref_ptr<osgText::Font> g_font = osgText::readFontFile("fonts/arial.ttf");
	osgText::Text* createText( const osg::Vec3& pos, const std::string&	content, float size )
	{
		osg::ref_ptr<osgText::Text> text = new osgText::Text;
		text->setDataVariance( osg::Object::DYNAMIC );
		text->setFont( g_font.get() );
		text->setCharacterSize( size );
		text->setAxisAlignment( osgText::TextBase::XY_PLANE );
		text->setPosition( pos );
		text->setText( content );
		return text.release();
	}
class PickHandler : public osgGA::GUIEventHandler
{
public:
	// This virtual method must be overrode by subclasses.
	virtual void doUserOperations(osgUtil::LineSegmentIntersector::Intersection& ) = 0;
	virtual bool handle( osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
	{
		if ( ea.getEventType()!=osgGA::GUIEventAdapter::RELEASE || ea.getButton()!=osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON || !(ea.getModKeyMask()&osgGA::GUIEventAdapter::MODKEY_CTRL) )
			return false;

		osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
		if ( viewer )
		{
			osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
			osgUtil::IntersectionVisitor iv( intersector.get() );
			viewer->getCamera()->accept( iv );

			if ( intersector->containsIntersections() )
			{
			//	osgUtil::LineSegmentIntersector::Intersection& result = *(intersector->getIntersections().begin());
				//doUserOperations( result );
			}
		}
		return false;
	}	
};
};

class RemoveShapeHandler : public osgCookBook::PickHandler
{
	virtual void doUserOperations( osgUtil::LineSegmentIntersector::Intersection& result )
	{
		if ( result.nodePath.size()>0 )
		{
			osg::Geode* geode = dynamic_cast<osg::Geode*>(result.nodePath.back() );
			if ( geode ) geode->removeDrawable(result.drawable.get() );
		}
	}
};

class ObserveShapeCallback : public osg::NodeCallback
{
public:
	virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
	{
		std::string content;
		if ( _drawable1.valid() ) content += "Drawable 1; ";
		if ( _drawable2.valid() ) content += "Drawable 2; ";
		if ( _text.valid() ) _text->setText( content );
	}
	osg::ref_ptr<osgText::Text> _text;
	osg::observer_ptr<osg::Drawable> _drawable1;
	osg::observer_ptr<osg::Drawable> _drawable2;
};

int main( int argc, char** argv )
{
	// Create the text and place it in an HUD camera
	osgCookBook osgcook;
	osgText::Text* text = osgcook.createText(osg::Vec3(50.0f, 50.0f, 0.0f), "", 10.0f);
	osg::ref_ptr<osg::Geode> textGeode = new osg::Geode;
	textGeode->addDrawable( text );

	osg::ref_ptr<osg::Camera> hudCamera = osgcook.createHUDCamera(0, 800, 0, 600);
	hudCamera->addChild( textGeode.get() );

	// Create two simple shapes and add both, as well as the camera,
	// to the root node
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable( new osg::ShapeDrawable(new osg::Box(osg::Vec3(-2.0f,0.0f,0.0f), 1.0f)) );
	geode->addDrawable( new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(2.0f,0.0f,0.0f), 1.0f)) );
	
	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild( hudCamera.get() );
	root->addChild( geode.get() );

	osg::ref_ptr<ObserveShapeCallback> observerCB = new ObserveShapeCallback;
	observerCB->_text = text;
	observerCB->_drawable1 = geode->getDrawable(0);
	observerCB->_drawable2 = geode->getDrawable(1);
	root->addUpdateCallback( observerCB.get() );

	osgViewer::Viewer viewer;
	viewer.addEventHandler( new RemoveShapeHandler );
	viewer.setSceneData( root.get() );
	return viewer.run();
}
/*
int main( int argc, char** argv )
{
	osg::ref_ptr<osg::Node> root = osgDB::readNodeFile("cessna.osg");

	osgViewer::Viewer viewer;
	viewer.setSceneData( root.get() );
	return viewer.run();
}
*/