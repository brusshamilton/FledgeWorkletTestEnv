/**
 * @fileoverview Description of this file.
 */

function generateBid(interestGroup, auctionSignals, perBuyerSignals, trustedBiddingSignals,
      browserSignals) {
  return {'render': interestGroup.ads[0].renderUrl, 'bid': 1};
}
