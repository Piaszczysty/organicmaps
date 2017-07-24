#pragma once

#include "map/bookmark.hpp"

#include "partners_api/taxi_provider.hpp"

#include "map/routing_mark.hpp"

#include "storage/index.hpp"

#include "indexer/feature_data.hpp"
#include "indexer/feature_meta.hpp"
#include "indexer/map_object.hpp"

#include "geometry/latlon.hpp"
#include "geometry/mercator.hpp"
#include "geometry/point2d.hpp"

#include <memory>
#include <string>
#include <vector>

namespace ads
{
struct Banner;
class Engine;
}

namespace place_page
{
enum class SponsoredType
{
  None,
  Booking,
  Opentable,
  Geochat,
  Viator,
  Cian
};

enum class LocalAdsStatus
{
  NotAvailable,
  Candidate,
  Customer
};

class Info : public osm::MapObject
{
public:
  static char const * const kSubtitleSeparator;
  static char const * const kStarSymbol;
  static char const * const kMountainSymbol;
  static char const * const kEmptyRatingSymbol;
  static char const * const kPricingSymbol;

  /// Place traits
  bool IsFeature() const { return m_featureID.IsValid(); }
  bool IsBookmark() const { return m_bac.IsValid(); }
  bool IsMyPosition() const { return m_isMyPosition; }
  bool IsRoutePoint() const { return m_isRoutePoint; }

  /// Edit and add
  bool ShouldShowAddPlace() const;
  bool ShouldShowAddBusiness() const { return m_canEditOrAdd && IsBuilding(); }
  bool ShouldShowEditPlace() const;

  /// UGC
  // TODO: Uncomment correct implementation.
  // TODO: UGC is disabled before UI isn't ready.
  bool ShouldShowUGC() const { return false; } //ftraits::UGC::IsUGCAvailable(m_types); }
  bool ShouldShowUGCRating() const { return false; } //ftraits::UGC::IsRatingAvailable(m_types); }
  bool ShouldShowUGCReviews() const { return false; } //ftraits::UGC::IsReviewsAvailable(m_types); }
  bool ShouldShowUGCDetails() const { return false; } //ftraits::UGC::IsDetailsAvailable(m_types); }

  /// @returns true if Back API button should be displayed.
  bool HasApiUrl() const { return !m_apiUrl.empty(); }
  /// TODO: Support all possible Internet types in UI. @See MapObject::GetInternet().
  bool HasWifi() const { return GetInternet() == osm::Internet::Wlan; }
  /// Should be used by UI code to generate cool name for new bookmarks.
  // TODO: Tune new bookmark name. May be add address or some other data.
  std::string FormatNewBookmarkName() const;

  /// For showing in UI
  std::string const & GetTitle() const { return m_uiTitle; };
  /// Convenient wrapper for secondary feature name.
  std::string const & GetSecondaryTitle() const { return m_uiSecondaryTitle; };
  /// Convenient wrapper for type, cuisines, elevation, stars, wifi etc.
  std::string const & GetSubtitle() const { return m_uiSubtitle; };
  std::string const & GetAddress() const { return m_uiAddress; }
  /// @returns coordinate in DMS format if isDMS is true
  std::string GetFormattedCoordinate(bool isDMS) const;
  /// @returns formatted rating string for booking object, or empty if it isn't booking object
  std::string GetRatingFormatted() const;
  /// @returns string with |kPricingSymbol| signs or empty std::string if it isn't booking object
  std::string GetApproximatePricing() const;

  /// UI setters
  void SetCustomName(std::string const & name);
  void SetAddress(std::string const & address) { m_address = address; }
  void SetIsMyPosition() { m_isMyPosition = true; }
  void SetCanEditOrAdd(bool canEditOrAdd) { m_canEditOrAdd = canEditOrAdd; }
  void SetLocalizedWifiString(std::string const & str) { m_localizedWifiString = str; }
  void SetLocalizedRatingString(std::string const & str) { m_localizedRatingString = str; }

  /// Bookmark
  BookmarkAndCategory const & GetBookmarkAndCategory() const { return m_bac; }
  std::string const & GetBookmarkCategoryName() const { return m_bookmarkCategoryName; }
  void SetBac(BookmarkAndCategory const & bac);
  void SetBookmarkCategoryName(std::string const & name) { m_bookmarkCategoryName = name; }
  void SetBookmarkData(BookmarkData const & data) { m_bookmarkData = data; }
  BookmarkData const & GetBookmarkData() const { return m_bookmarkData; }

  /// Api
  void SetApiId(std::string const & apiId) { m_apiId = apiId; }
  void SetApiUrl(std::string const & url) { m_apiUrl = url; }
  std::string const & GetApiUrl() const { return m_apiUrl; }

  /// Sponsored
  bool IsSponsored() const { return m_sponsoredType != SponsoredType::None; }
  bool IsNotEditableSponsored() const { return m_sponsoredType == SponsoredType::Booking; }
  void SetBookingSearchUrl(std::string const & url) { m_bookingSearchUrl = url; }
  std::string const & GetBookingSearchUrl() const { return m_bookingSearchUrl; }
  void SetSponsoredUrl(std::string const & url) { m_sponsoredUrl = url; }
  std::string const & GetSponsoredUrl() const { return m_sponsoredUrl; }
  void SetSponsoredDescriptionUrl(std::string const & url) { m_sponsoredDescriptionUrl = url; }
  std::string const & GetSponsoredDescriptionUrl() const { return m_sponsoredDescriptionUrl; }
  void SetSponsoredReviewUrl(std::string const & url) { m_sponsoredReviewUrl = url; }
  std::string const & GetSponsoredReviewUrl() const { return m_sponsoredReviewUrl; }
  void SetSponsoredType(SponsoredType type) { m_sponsoredType = type; }
  SponsoredType GetSponsoredType() const { return m_sponsoredType; }

  /// Banner
  bool HasBanner() const;
  std::vector<ads::Banner> GetBanners() const;

  /// Taxi
  void SetReachableByTaxiProviders(std::vector<taxi::Provider::Type> const & providers)
  {
    m_reachableByProviders = providers;
  }

  std::vector<taxi::Provider::Type> const & ReachableByTaxiProviders() const
  {
    return m_reachableByProviders;
  }

  /// Local ads
  void SetLocalAdsStatus(LocalAdsStatus status) { m_localAdsStatus = status; }
  LocalAdsStatus GetLocalAdsStatus() const { return m_localAdsStatus; }
  void SetLocalAdsUrl(std::string const & url) { m_localAdsUrl = url; }
  std::string const & GetLocalAdsUrl() const { return m_localAdsUrl; }
  void SetAdsEngine(ads::Engine * const engine) { m_adsEngine = engine; }

  /// Routing
  void SetRouteMarkType(RouteMarkType type) { m_routeMarkType = type; }
  RouteMarkType GetRouteMarkType() const { return m_routeMarkType; }
  void SetIntermediateIndex(int8_t index) { m_intermediateIndex = index; }
  int8_t GetIntermediateIndex() const { return m_intermediateIndex; }
  void SetIsRoutePoint() { m_isRoutePoint = true; }

  /// CountryId
  /// Which mwm this MapObject is in.
  /// Exception: for a country-name point it will be set to the topmost node for the mwm.
  /// TODO(@a): use m_topmostCountryIds in exceptional case.
  void SetCountryId(storage::TCountryId const & countryId) { m_countryId = countryId; }
  storage::TCountryId const & GetCountryId() const { return m_countryId; }
  void SetTopmostCountryIds(storage::TCountriesVec const & ids) { m_topmostCountryIds = ids; }

  /// MapObject
  void SetFromFeatureType(FeatureType const & ft);

  void SetMercator(m2::PointD const & mercator) { m_mercator = mercator; }
  std::vector<std::string> GetRawTypes() const { return m_types.ToObjectNames(); }

private:
  std::string FormatSubtitle(bool withType) const;
  void GetPrefferedNames(std::string & primaryName, std::string & secondaryName) const;
  /// @returns empty string or GetStars() count of ★ symbol.
  std::string FormatStars() const;

  /// UI
  std::string m_uiTitle;
  std::string m_uiSubtitle;
  std::string m_uiSecondaryTitle;
  std::string m_uiAddress;
  // TODO(AlexZ): Temporary solution. It's better to use a wifi icon in UI instead of text.
  std::string m_localizedWifiString;
  /// Booking rating string
  std::string m_localizedRatingString;

  /// CountryId
  storage::TCountryId m_countryId = storage::kInvalidCountryId;
  /// The topmost downloader nodes this MapObject is in, i.e.
  /// the country name for an object whose mwm represents only
  /// one part of the country (or several countries for disputed territories).
  storage::TCountriesVec m_topmostCountryIds;

  /// Comes from API, shared links etc.
  std::string m_customName;

  /// Bookmarks
  /// If not empty, bookmark is bound to this place page.
  BookmarkAndCategory m_bac;
  /// Bookmark category name. Empty, if it's not bookmark;
  std::string m_bookmarkCategoryName;
  BookmarkData m_bookmarkData;

  /// Api ID passed for the selected object. It's automatically included in api url below.
  std::string m_apiId;
  /// [Deep] link to open when "Back" button is pressed in a Place Page.
  std::string m_apiUrl;
  /// Formatted feature address for inner using.
  std::string m_address;

  /// Routing
  RouteMarkType m_routeMarkType;
  int8_t m_intermediateIndex = 0;
  bool m_isRoutePoint = false;

  bool m_isMyPosition = false;

  /// Editor
  /// True if editing of a selected point is allowed by basic logic.
  /// See initialization in framework.
  bool m_canEditOrAdd = false;

  /// Ads
  std::vector<taxi::Provider::Type> m_reachableByProviders;
  std::string m_localAdsUrl;
  LocalAdsStatus m_localAdsStatus = LocalAdsStatus::NotAvailable;
  /// Ads source.
  ads::Engine * m_adsEngine = nullptr;
  /// Sponsored type or None.
  SponsoredType m_sponsoredType = SponsoredType::None;

  /// Sponsored feature urls.
  std::string m_sponsoredUrl;
  std::string m_sponsoredDescriptionUrl;
  std::string m_sponsoredReviewUrl;

  /// Booking
  std::string m_bookingSearchUrl;
};
}  // namespace place_page
