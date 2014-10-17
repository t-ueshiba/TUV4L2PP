/*
 *  $Id$
 */
#ifdef HAVE_LIBTUTOOLS__
#include <cstdlib>
#include <iomanip>
#include "TU/Ieee1394CameraArray.h"
#include "TU/io.h"

namespace TU
{
/************************************************************************
*  class Ieee1394CameraArray						*
************************************************************************/
//! 空のIEEE1394デジタルカメラの配列を生成する.
Ieee1394CameraArray::Ieee1394CameraArray()
    :Array<Ieee1394Camera*>()
{
}
    
//! IEEE1394デジタルカメラの配列を生成する.
/*!
  \param name		カメラ名
  \param dirs		カメラ設定ファイルの探索ディレクトリ名の並び
  \param i1394b		IEEE1394bモード (800Mbps)で動作
  \param ncameras	生成するカメラ台数. 設定ファイルに記されている最初の
			ncameras台が生成される. -1を指定すると, 設定ファイル
			中の全カメラが生成される. 
*/
Ieee1394CameraArray::Ieee1394CameraArray(const char* name, const char* dirs,
					 Ieee1394Node::Speed speed,
					 int ncameras)
    :Array<Ieee1394Camera*>(), _fullName()
{
    initialize(name, dirs, speed, ncameras);
}
    
//! IEEE1394デジタルカメラの配列を初期化する.
/*!
  \param name		カメラ名
  \param dirs		カメラ設定ファイルの探索ディレクトリ名の並び
  \param i1394b		IEEE1394bモード (800Mbps)で動作
  \param ncameras	生成するカメラ台数. 設定ファイルに記されている最初の
			ncameras台が生成される. -1を指定すると, 設定ファイル
			中の全カメラが生成される. 
*/
void
Ieee1394CameraArray::initialize(const char* name, const char* dirs,
				Ieee1394Node::Speed speed, int ncameras)
{
  // 設定ファイルのfull path名を生成し, ファイルをオープンする.
    std::ifstream	in;
    _fullName = openFile(in,
			 std::string(name != 0 ? name : DEFAULT_CAMERA_NAME),
			 std::string(dirs != 0 ? dirs : DEFAULT_CONFIG_DIRS),
			 ".conf");
    
  // 設定ファイルから遅延パラメータとカメラ数を読み込む.
    int	delay, n;
    in >> delay >> n;
    if ((ncameras < 0) || (ncameras > n))
	ncameras = n;

    restore(in, ncameras, speed, delay);
}

//! IEEE1394デジタルカメラの配列を破壊する.
Ieee1394CameraArray::~Ieee1394CameraArray()
{
    for (size_t i = 0; i < size(); ++i)
	delete (*this)[i];
}

std::istream&
Ieee1394CameraArray::restore(std::istream& in, int ncameras,
			     Ieee1394Node::Speed speed, int delay)
{
  // 現在設定されている全カメラを廃棄する.
    for (size_t i = 0; i < size(); ++i)
	delete (*this)[i];

  // カメラ数を設定する．
    resize(ncameras);
    
  // 設定ファイルに記された全カメラを生成する.
    for (size_t i = 0; i < size(); ++i)
    {
	std::string	s;
	in >> s;			// global unique IDの読み込み
	u_int64_t	uniqId = strtoull(s.c_str(), 0, 0);
	(*this)[i] = new Ieee1394Camera(Ieee1394Camera::Monocular,
					uniqId, speed, delay);
	in >> *(*this)[i];		// カメラパラメータの読み込みと設定
    }

    return in;
}
    
/************************************************************************
*  global functions							*
************************************************************************/
std::istream&
operator >>(std::istream& in, Ieee1394CameraArray& cameras)
{
    int	delay, n;
    in >> delay >> n;		// 遅延パラメータとカメラ数を読み込む.

    return cameras.restore(in, n, Ieee1394Node::SPD_400M, delay);
}

std::ostream&
operator <<(std::ostream& out, const Ieee1394CameraArray& cameras)
{
    using namespace	std;
    
    out << cameras[0]->delay() << ' ' << cameras.size() << endl;
    for (size_t i = 0; i < cameras.size(); ++i)
	out << "0x" << setw(16) << setfill('0')
	    << hex << cameras[i]->globalUniqueId() << ' '
	    << dec << *cameras[i];

    return out;
}

}
#endif	/* HAVE_LIBTUTOOLS__	*/    
