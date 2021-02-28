#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <qt5/QtCore/QtCoreDepends>
#include <osgGA/TrackballManipulator>
#include <osgQt/GraphicsWindowQt>

//#include <qt5/QtGui/QtGui>>

#include <qt5/QtCore/QtCore>

osg::Camera* createCamera( int x, int y, int w, int h )
{
	osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
	osg::ref_ptr<osg::GraphicsContext::Traits> traits =	new osg::GraphicsContext::Traits;
	traits->windowDecoration = false;
	traits->x = x;
	traits->y = y;
	traits->width = w;
	traits->height = h;
	traits->doubleBuffer = true;
	osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	camera->setClearColor( osg::Vec4(0.2, 0.2, 0.6, 1.0) );
	camera->setViewport( new osg::Viewport( 0, 0, traits->width, traits->height) );
	camera->setProjectionMatrixAsPerspective( 30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0f, 10000.0f );
	return camera.release();
}

class ViewerWidget : public QWidget
{
public:
	ViewerWidget( osg::Camera* camera, osg::Node* scene );
	protected:
	virtual void paintEvent( QPaintEvent* event )
	{ _viewer.frame(); }
	osgViewer::Viewer _viewer;
	QTimer _timer;
	};

int main( int argc, char** argv )
{
	osgViewer::Viewer viewer;
	viewer.setSceneData( osgDB::readNodeFile("cessna.osg") );
	return viewer.run();
}
