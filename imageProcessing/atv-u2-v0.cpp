/**
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/
/**
 * @file atv-u2-v0.cpp
 * @ingroup Tools
 * @author Jacques-Olivier Lachaud (\c jacques-olivier.lachaud@univ-savoie.fr )
 * Laboratory of Mathematics (CNRS, UMR 5127), University of Savoie, France
 * @author Marion Foare (\c marion.foare@univ-savoie.fr )
 * Laboratory of Mathematics (CNRS, UMR 5127), University of Savoie, France
 *
 * @date 2016/10/12
 *
 * A tool file named atv-u2-v0.
 *
 * This file is part of the DGtal library.
 */

///////////////////////////////////////////////////////////////////////////////
#include <iostream>

#include <sstream>
#include <string>
#include <functional>
#include <boost/format.hpp>

#include "CLI11.hpp"

#include "DGtal/base/Common.h"
#include "DGtal/helpers/StdDefs.h"
#include "DGtal/io/readers/GenericReader.h"
#include "DGtal/io/writers/GenericWriter.h"

#include "ATVu2v0.h"

/**
@page DocATVu2v0 imageProcessing/atv-u2-v0 

@brief Computes a piecewise smooth approximation of a grey-level or color image, by optimizing a variant of the Ambrosio-Tortorelli functional (with u a 2-form and v a 0-form).

@writers Jacques-Olivier Lachaud

@b Usage: atv-u2-v0 -i [input.pgm]

(for grey-level image restoration)

@b Usage: atv-u2-v0 -i [input.ppm]

(for color image restoration)

The Ambrosio-Tortorelli functional is a classical relaxation of the
Mumford-Shah functional.

Given an input grayscale image, defined in an open bounded domain
\f$ \Omega \f$, we represent its gray levels by a function \f$ g
\in L^{\infty}(\Omega) \f$. In the Ambrosio-Tortorelli functional [1],
one wants to find a function \f$ u \in SBV(\Omega) \f$ which is a
smooth approximation of the input image \f$ g \f$.
The Ambrosio-Tortorelli functional [1] is defined by
\f[
  \displaystyle
  AT_{\varepsilon}(u,v)	= \int_\Omega \alpha |u-g|^2 + v^2 |\nabla u|^2
  + \lambda \varepsilon |\nabla v|^2 + \frac{\lambda}{4 \varepsilon} |1-v|^2 dx,
\f]
for functions \f$ u,v \in W^{1,2}(\Omega)\f$ with \f$ 0 \leq v \leq 1 \f$.
We use a variant here that reads as:
\f[
  \displaystyle
  ATV_{\varepsilon}(u,v) = \int_\Omega \alpha |u-g|^2 + v^2 |\nabla u|^2
  + c \lambda \varepsilon |\Delta v|^2 + c \frac{\lambda}{\varepsilon} |1-v|^2 dx,
\f]

It \f$ \Gamma \f$-converges to a length estimation multiplied by \f$2
\sqrt{2}\f$, while having a smoother profile than the standard AT. The
constant c is set to \f$1/(2 \sqrt{2})\f$ to correct this estimation
and to keep the same parameterization as AT.


In ATV functional, function \f$ v \f$ is a smooth approximation
of the set of discontinuities, and takes value close to 0 in this set,
while being close to 1 outside discontinuities. A remarkable property
of this functional is that it \f$ \Gamma \f$-converges to (a
relaxation of) MS functional as \f$ \varepsilon \f$ tends to 0 (see [1]).
The intuition is that a large \f$ \varepsilon \f$ induces a solution
with a fuzzy set of discontinuities, which is then progressively
narrowed to the crisp 1-dimensional set of discontinuites as
\f$ \varepsilon \f$ goes to 0.

We discretize ATV with discrete calculus and we set \f$ u \f$ and \f$ g
\f$ to live on the faces and \f$ v \f$ to live on the vertices and
edges. Pixels are faces, so functions \f$ u \f$ and \f$ g \f$ are
2-forms since they represent the gray levels of each pixel. On the
contrary, we set \f$ v \f$ in-between cells of non null measure, so in
this case on vertices as a 0-form, and on edges by averaging with \f$
\mathbf{M} \f$. We call this formulation ATV20. The DEC reformulation
is straightforward, except for the second term, where we use matrix
\f$ \mathbf{M} \f$ to transport the 0-form \f$ v \f$ onto edges :

\f[
  \displaystyle
  ATV20(u,v) = \Sigma_{i=1}^n
      \alpha \langle u_i - g_i , u_i - g_i \rangle_2
    + \langle \mathbf{M} v , \bar{\mathbf{\star}} \bar{\mathbf{d_0}}
      \mathbf{\star} u_i \rangle_1 ^2 \\
    + c \lambda \varepsilon \langle \mathbf{L_0} v , \mathbf{L_0} v \rangle_1
    + c \frac{\lambda}{4\varepsilon} \langle 1 - v , 1 - v \rangle_0.
\f]

For more comparisons and details, see \ref moduleAT

\b Allowed \b options \b are:

\code
 
 Positionals:
   1 TEXT:FILE REQUIRED                  the input image PPM filename.

 Options:
   -h,--help                             Print this help message and exit
   -i,--input TEXT:FILE REQUIRED         the input image PPM filename.
   -m,--inpainting-mask TEXT             the input inpainting mask filename.
   -o,--output TEXT=AT                   the output image basename.
   -l,--lambda FLOAT                     the parameter lambda.
   -M,--metric-average                   use metric average to smooth L1-metric.
   -1,--lambda-1 FLOAT=0.3125            the initial parameter lambda (l1).
   -2,--lambda-2 FLOAT=0.0005            the final parameter lambda (l2).
   -q,--lambda-ratio FLOAT=1.41421       the division ratio for lambda from l1 to l2.
   -a,--alpha FLOAT=1                    the parameter alpha.
   -e,--epsilon                          the initial and final parameter epsilon of AT functional at the same time.
   --epsilon-1 FLOAT=2                   the initial parameter epsilon.
   --epsilon-2 FLOAT=0.25                the final parameter epsilon.
   --epsilon-r FLOAT=2                   sets the ratio between two consecutive epsilon values of AT functional.
   -n,--nbiter INT=10                    the maximum number of iterations.
   --image-snr TEXT                      the input image without deterioration if you wish to compute the SNR.
   -p,--pixel-size INT=1                 the pixel size for outputing images (useful when one wants to see the discontinuities v on top of u).
   -c,--color-v TEXT=0xff0000            the color chosen for displaying the singularities v (e.g. red is 0xff0000).
   -v,--verbose INT=0                    the verbose level (0: silent, 1: less silent, etc).
 
\endcode

@b example:

\code
./imageProcessing/atv-u2-v0 -i ../imageProcessing/Images/degrade-b04.pgm --image-snr ../imageProcessing/Images/degrade.pgm -a 0.05 --epsilon-1 4 --epsilon-2 0.25 -l 0.0075 -p 2 -c 0xff0000 -o degrade
\endcode

<center>
<table>
<tr>
<td> Input image \a g </td>
<td> Reconstructed image \a u </td>
<td> Perfect image </td>
<td> Reconstructed image \a u (with discontinuities \a v)</td>
</tr>
<tr>
<td>@image html degrade-b04.png "Input image (noise = 0.4)"</td>
<td>@image html degrade-atv-a0.05000-l0.0075000-u.png "ATV20 alpha=0.05 lambda=0.0075 "</td>
<td>@image html degrade.png "Perfect image"</td>
<td>@image html degrade-atv-a0.05000-l0.0075000-u-v.png "ATV20 alpha=0.05 lambda=0.0075 "</td>
</tr>
<tr>
<td> SNR of \a g = 21.9183 </td>
<td> SNR of \a u = 34.3261 </td>
<td> Perfect image </td>
<td> SNR of \a u = 34.3261 </td>
</tr>
</table>
</center>

@note Other restoration examples, parameter analysis, and image
inpainting examples may be found in \ref moduleAT.

[1] Luigi Ambrosio, and Vincenzo Maria Tortorelli. "Approximation of
functional depending on jumps by elliptic functional via
\f$\Gamma\f$‐convergence."  Communications on Pure and Applied
Mathematics 43.8 (1990): 999-1036.

[2] Marion Foare, Jacques-Olivier Lachaud, and Hugues Talbot. "Image restoration
and segmentation using the Ambrosio-Tortorelli functional and discrete calculus."
In Proceedings of the IAPR International Conference on Pattern Recognition
(ICPR2016), Cancun, Mexico, 2016.

[3] Marion Foare, Jacques-Olivier Lachaud, and Hugues
Talbot. "Numerical implementation of the Ambrosio-Tortorelli
functional using discrete calculus and application to image
restoration and inpainting."  In Proceedings of 1st Workshop on
Reproducible Research In Pattern Recognition (RRPR 2016), Springer
LNCS. To appear.

[4] Matteo Focardi. "On the variational approximation of free-
discontinuity problems in the vectorial case." Mathematical Models
and Methods in Applied Sciences 11.04 (2001): 663-684.

*/


using namespace std;
using namespace DGtal;

int main( int argc, char* argv[] )
{
  using namespace Z2i;

  // parse command line ----------------------------------------------
  CLI::App app;
  string f1;
  string f2 {"AT"};
  string inpainting_mask;
  double l;
  double l1 {0.3125};
  double l2 {0.0005};
  double lr {sqrt(2)};
  double a   {1.0};
  double epsilon;
  
  double e1  {2.0};
  double e2 {0.25};
  double er  {2.0};
  int  verb  {0};
  int nbiter = {10};
  int pix_sz = {1};
  string scv {"0xff0000"};
  string isnr;
  bool metric;
  
  
  stringstream ssDescr;
  ssDescr << "Usage: " << argv[0] << " -i toto.pgm\n"
  << "Computes a variant of the Ambrosio-Tortorelli reconstruction/segmentation of an input image."
  << "It outputs 2 or 3 images (of basename given by option --output) giving the"
  << " reconstructed image u, and other images superposing u and the discontinuities v."
  << endl << endl
  << " / "
  << endl
  << " | a.(u-g)^2 + v^2 |grad u|^2 + c.l.e^3.|Delta v|^2 + (c.l/e).(1-v)^2 "
  << endl
  << " / "
  << endl
  << "Discretized as (u 2-form, v 0-form, L=A^t A laplacian, B edge-face bdy, M vertex-edge average)" << endl
  << "E(u,v) = a(u-g)^t (u-g) +  u^t B diag(M v)^2 B^t u + l e^3 v^t L^t L v + l/e (1-v)^t (1-v)" << endl
  << endl
<< "where c=1/(2sqrt(2)) is a constant that reflects the fact that the Gamma-limit of discontinuity length estimation term is 2.sqrt(2).L, for a length L of discontinuities."
  << "Example: ./atv-u2-v0 -i ../Images/cerclesTriangle64b02.pgm -o tmp -a 0.05 -e 1 --lambda-1 0.1 --lambda-2 0.00001"
  << endl;
  
  
  app.description(ssDescr.str());


app.add_option("-i,--input,1", f1, "the input image PPM filename." )
    ->required()
    ->check(CLI::ExistingFile);
app.add_option("--inpainting-mask,-m", inpainting_mask, "the input inpainting mask filename." );
app.add_option("--output,-o", f2, "the output image basename.", true);
auto lambdaOpt = app.add_option("--lambda,-l",l, "the parameter lambda.");
app.add_flag("--metric-average,-M",metric, "use metric average to smooth L1-metric." );

app.add_option("--lambda-1,-1",l1, "the initial parameter lambda (l1).", true);
app.add_option("--lambda-2,-2",l2, "the final parameter lambda (l2).", true );
app.add_option("--lambda-ratio,-q",lr,  "the division ratio for lambda from l1 to l2.", true);
app.add_option("--alpha,-a",a, "the parameter alpha.", true);
auto epsOpt = app.add_option("--epsilon,-e", "the initial and final parameter epsilon of AT functional at the same time.");

app.add_option("--epsilon-1",e1, "the initial parameter epsilon.", true);
app.add_option("--epsilon-2",e2, "the final parameter epsilon.", true);
app.add_option("--epsilon-r",er,  "sets the ratio between two consecutive epsilon values of AT functional.", true);

app.add_option("--nbiter,-n",nbiter, "the maximum number of iterations.", true );
auto snrOpt = app.add_option("--image-snr", isnr, "the input image without deterioration if you wish to compute the SNR.");
app.add_option("--pixel-size,-p", pix_sz, "the pixel size for outputing images (useful when one wants to see the discontinuities v on top of u).", true);
app.add_option("--color-v,-c",scv, "the color chosen for displaying the singularities v (e.g. red is 0xff0000).", true );
app.add_option("--verbose,-v", verb, "the verbose level (0: silent, 1: less silent, etc).", true );

app.get_formatter()->column_width(40);
CLI11_PARSE(app, argc, argv);
// END parse comm"metric-average,Mand line using CLI ----------------------------------------------


  Color color_v( (unsigned int) std::stoul( scv, nullptr, 16 ), 255 );
  if ( lambdaOpt->count()) l1 = l2 = l;
  if ( l2 > l1 ) l2 = l1;
  if ( lr <= 1.0 ) lr = sqrt(2);
  if ( epsOpt->count() > 0 ){
    e1 = e2 =  epsilon;

  }
  bool snr   = snrOpt->count() > 0;

  
  bool color_image = f1.size() > 4 && f1.compare( f1.size() - 4, 4, ".ppm" ) == 0;
  bool grey_image  = f1.size() > 4 && f1.compare( f1.size() - 4, 4, ".pgm" ) == 0;
  if ( ! color_image && ! grey_image )
    {
      trace.error() << "Input image file must be either a PGM (grey-level) or a PPM (color) image with these extensions."
                    << endl;
      return 2;
    }


  KSpace K;
  ATVu2v0< KSpace > AT( verb );
  Domain domain;
  AT.setMetricAverage( metric );
  
  typedef ATVu2v0<KSpace>::Calculus Calculus;
  typedef ImageContainerBySTLVector<Domain, Color> ColorImage;
  typedef ImageContainerBySTLVector<Domain, unsigned char> GreyLevelImage;
  //---------------------------------------------------------------------------
  if ( color_image ) 
    {
      trace.beginBlock("Reading PPM image");
      ColorImage image = PPMReader<ColorImage>::importPPM( f1 );
      trace.endBlock();
      trace.beginBlock("Building AT");
      domain = image.domain();
      K.init( domain.lowerBound(), domain.upperBound(), true );
      AT.init( K );
      AT.addInput( image, [] ( Color c ) -> double { return ((double) c.red())   / 255.0; } );
      AT.addInput( image, [] ( Color c ) -> double { return ((double) c.green()) / 255.0; } );
      AT.addInput( image, [] ( Color c ) -> double { return ((double) c.blue())  / 255.0; } );
      trace.endBlock();
    }
  else if ( grey_image ) 
    {
      trace.beginBlock("Reading PGM image");
      GreyLevelImage image = PGMReader<GreyLevelImage>::importPGM( f1 );
      trace.endBlock();
      trace.beginBlock("Building AT");
      domain = image.domain();
      K.init( domain.lowerBound(), domain.upperBound(), true );
      AT.init( K );
      AT.addInput( image, [] (unsigned char c ) { return ((double) c) / 255.0; } );
      trace.endBlock();
    }

  //---------------------------------------------------------------------------
  if ( snr && color_image ) 
    {
      trace.beginBlock("Reading ideal PPM image");
      ColorImage image = PPMReader<ColorImage>::importPPM( isnr );
      trace.endBlock();
      AT.addInput( image, [] ( Color c ) -> double { return ((double) c.red())   / 255.0; }, true );
      AT.addInput( image, [] ( Color c ) -> double { return ((double) c.green()) / 255.0; }, true );
      AT.addInput( image, [] ( Color c ) -> double { return ((double) c.blue())  / 255.0; }, true );
    }
  else if ( snr && grey_image ) 
    {
      trace.beginBlock("Reading ideal PGM image");
      GreyLevelImage image = PGMReader<GreyLevelImage>::importPGM( isnr );
      trace.endBlock();
      AT.addInput( image, [] (unsigned char c ) { return ((double) c) / 255.0; }, true );
    }
  
  //---------------------------------------------------------------------------
  // Prepare zoomed output domain
  Domain out_domain( pix_sz * domain.lowerBound(), 
                     pix_sz * domain.upperBound() + Point::diagonal( pix_sz ) );
  //---------------------------------------------------------------------------
  AT.setUFromInput();
  double g_snr = snr ? AT.computeSNR() : 0.0;

  if ( inpainting_mask.size() > 0 )
    {
      string fm  = inpainting_mask;
      trace.beginBlock("Reading inpainting mask");
      GreyLevelImage mask = GenericReader<GreyLevelImage>::import( fm );
      trace.endBlock();
      Calculus::PrimalForm2 m( AT.calculus );
      for ( Calculus::Index index = 0; index < m.myContainer.rows(); index++)
        {
          auto cell = m.getSCell( index );
          double col = ((double) mask( K.sCoords( cell ) )) / 255.0;
          m.myContainer( index ) = col > 0.0 ? 1.0 : 0.0;
        }
      AT.setAlpha( a, m );
      AT.setUFromInputAndMask();
      if ( grey_image )
        {
          ostringstream ossGM;
          ossGM << boost::format("%s-g-mask.pgm") %f2;
          GreyLevelImage image_mg( domain );
          const Calculus::PrimalForm2 mg = functions::dec::diagonal( m ) * AT.getG( 0 );
          functions::dec::form2ToGreyLevelImage
            ( AT.calculus, mg, image_mg, 0.0, 1.0, 1 ); 
          PGMWriter<GreyLevelImage>::exportPGM( ossGM.str(), image_mg );
        }
      else if ( color_image )
        {
          ostringstream ossGM;
          ossGM << boost::format("%s-g-mask.ppm") %f2;
          ColorImage image_mg( domain );
          const Calculus::PrimalForm2 mg0 = functions::dec::diagonal( m ) * AT.getG( 0 );
          const Calculus::PrimalForm2 mg1 = functions::dec::diagonal( m ) * AT.getG( 1 );
          const Calculus::PrimalForm2 mg2 = functions::dec::diagonal( m ) * AT.getG( 2 );
          functions::dec::threeForms2ToRGBColorImage
            ( AT.calculus, mg0, mg1, mg2, image_mg, 0.0, 1.0, 1 ); 
          PPMWriter<ColorImage, functors::Identity >::exportPPM( ossGM.str(), image_mg );
        }
    }
  else 
    AT.setAlpha( a );
  
  trace.info() << AT << std::endl;
  double n_v = 0.0;
  double eps = 0.0;
  double cst = 1.0 / ( 2.0 * sqrt(2.0) );
  while ( l1 >= l2 )
    {
      trace.info() << "************ lambda = " << l1 << " **************" << endl;
      AT.setLambda( cst * l1 );
      for ( eps = e1; eps >= e2; eps /= er )
        {
          trace.info() << "  ======= epsilon = " << eps << " ========" << endl;
          AT.setEpsilon( eps );
          int n = 0;
          do {
            trace.progressBar( n, nbiter );
            AT.solveU();
            AT.solveV();
            AT.checkV();
            n_v = AT.computeVariation();
          } while ( ( n_v > 0.0001 ) && ( ++n < nbiter ) );
          trace.progressBar( n, nbiter );
          trace.info() << "[#### last variation = " << n_v << " " << endl;
        }
      if ( grey_image ) 
        {
          if ( verb > 0 ) trace.beginBlock("Writing u[0] as PGM image");
          ostringstream ossU, ossV, ossW;
          ossU << boost::format("%s-a%.5f-l%.7f-u.pgm") % f2 % a % l1;
          ossV << boost::format("%s-a%.5f-l%.7f-u-v.pgm") % f2 % a % l1;
          ossW << boost::format("%s-a%.5f-l%.7f-u-v.ppm") % f2 % a % l1;
          const Calculus::PrimalForm2 u = AT.getU( 0 );
          const Calculus::PrimalForm1 v = AT.M01 * AT.getV();
          // Restored image
          GreyLevelImage image_u( domain );
          functions::dec::form2ToGreyLevelImage
            ( AT.calculus, u, image_u, 0.0, 1.0, 1 ); 
          PGMWriter<GreyLevelImage>::exportPGM( ossU.str(), image_u );
          // Zoomed restored image with discontinuities (in black).
          GreyLevelImage image_uv( out_domain );
          functions::dec::form2ToGreyLevelImage
            ( AT.calculus, u, image_uv, 0.0, 1.0, pix_sz ); 
          functions::dec::primalForm1ToGreyLevelImage
            ( AT.calculus, v, image_uv, 0.0, 1.0, pix_sz ); 
          PGMWriter<GreyLevelImage>::exportPGM( ossV.str(), image_uv );
          // Zoomed restored image with discontinuities (in specified color).
          ColorImage cimage( out_domain );
          functions::dec::threeForms2ToRGBColorImage
            ( AT.calculus, u, u, u, cimage, 0.0, 1.0, pix_sz ); 
          functions::dec::primalForm1ToRGBColorImage
            ( AT.calculus, v, cimage, color_v, 0.0, 1.0, pix_sz ); 
          PPMWriter<ColorImage, functors::Identity >::exportPPM( ossW.str(), cimage );
          if ( verb > 0 ) trace.endBlock();
        }
      else if ( color_image )
        {
          if ( verb > 0 ) trace.beginBlock("Writing u[0,1,2] as PGM image");
          ostringstream ossU, ossV;
          ossU << boost::format("%s-a%.5f-l%.7f-u.ppm") % f2 % a % l1;
          ossV << boost::format("%s-a%.5f-l%.7f-u-v.ppm") % f2 % a % l1;
          const Calculus::PrimalForm2 u0 = AT.getU( 0 );
          const Calculus::PrimalForm2 u1 = AT.getU( 1 );
          const Calculus::PrimalForm2 u2 = AT.getU( 2 );
          const Calculus::PrimalForm1 v  = AT.M01 * AT.getV();
          // Restored image
          ColorImage image_u( domain );
          functions::dec::threeForms2ToRGBColorImage
            ( AT.calculus, u0, u1, u2, image_u, 0.0, 1.0, 1 ); 
          PPMWriter<ColorImage, functors::Identity >::exportPPM( ossU.str(), image_u );
          ColorImage image_uv( out_domain );
          functions::dec::threeForms2ToRGBColorImage
            ( AT.calculus, u0, u1, u2, image_uv, 0.0, 1.0, pix_sz ); 
          functions::dec::primalForm1ToRGBColorImage
            ( AT.calculus, v, image_uv, color_v, 0.0, 1.0, pix_sz ); 
          PPMWriter<ColorImage, functors::Identity >::exportPPM( ossV.str(), image_uv );
          if ( verb > 0 ) trace.endBlock();
        }
      // Compute SNR if possible
      if ( snr )
        {
          double u_snr = AT.computeSNR();
          trace.info() << "- SNR of u = " << u_snr << "   SNR of g = " << g_snr << endl;
        }
      l1 /= lr;
    }
  return 0;
}
