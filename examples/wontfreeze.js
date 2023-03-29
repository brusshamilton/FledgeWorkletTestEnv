/**
 * @fileoverview Description of this file.
 */

const incrementer = (function() {
  let a = 1;
 return function() { a += 1; return a; };
})();

function generateBid(interestGroup, auctionSignals, perBuyerSignals, trustedBiddingSignals,
      browserSignals) {

  return {'render': interestGroup.ads[0].renderUrl, 'bid': incrementer()};
}
