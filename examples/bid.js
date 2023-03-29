/**
 * @fileoverview Description of this file.
 */

function generateBid(interestGroup, auctionSignals, perBuyerSignals, trustedBiddingSignals,
      browserSignals) {
  setBid({'render': interestGroup.ads[0].renderUrl, 'bid': 1});
  console.log("interestGroup: " + JSON.stringify(interestGroup));
  console.log("auctionSignals: " + JSON.stringify(auctionSignals));
  console.log("perBuyerSignals: " + JSON.stringify(perBuyerSignals));
  console.log("trustedBiddingSignals: " + JSON.stringify(trustedBiddingSignals));
  console.log("browserSignals: " + JSON.stringify(browserSignals));
  return {'render': interestGroup.ads[0].renderUrl, 'bid': 1};
}
